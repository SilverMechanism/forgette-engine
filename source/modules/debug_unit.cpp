module;
#include "defines.h"
export module debug_unit;

import core;
import unit;
import input;
import std;
import movement_element;
import input_handler;
import sprite_sheet;
import directx;
import debug_device;
import forgette;
import helpers;
import health_element;
import collision_element;

const std::unordered_map<std::string, std::uint8_t> angles =
{
	{"down", 1},
	{"down_left", 2},
	{"left", 3},
	{"up_left", 4},
	{"up", 5},
	{"up_right", 6},
	{"right", 7},
	{"down_right", 8}
};

export
{
	SCRIPTABLE_UNIT(Debug)
	class DebugUnit : public Unit, public InputHandler
	{
	public:
		DebugUnit();
		
		ptr::watcher<DebugDevice> debug_device;
		
		virtual void bind_inputs() override;
		
		virtual void game_update(float delta_time) override;
		
		std::unordered_map<std::string, std::function<void()>> binds;
		
		virtual void on_spawn() override;
		
		ptr::keeper<SpriteSheet> sprite_sheet;
		
		int frame = 0;
		
		int to_sheet_frame(coordinates<float> vec);
		
		virtual void on_death() override;
		
	protected:
		void set_sprite_direction();
		void set_sprite_direction_mouse();
		
	private:
	};
}

DebugUnit::DebugUnit()
{
	add_entity_identifier(EntityClass::DebugUnit);
	
	display_name = "Debug Unit";
	sprite_name = "waifu";
	
	should_game_update = true;
	add_element<MovementElement>();
	
	add_element<HealthElement>();
	
	add_element<CollisionElement>();
	CollisionElement* ce = get_element<CollisionElement>();
	
	ce->collides_with.insert(CollisionGroup::Unit);
	ce->collides_with.insert(CollisionGroup::Projectile);
	ce->collides_with.insert(CollisionGroup::Prop);
	
	get_element<MovementElement>()->walk_speed = 72.0f;
}

void DebugUnit::game_update(float delta_time)
{
	Unit::game_update(delta_time);
	
	/* if (sprite_sheet.get() && sprite.get() && movement.velocity.magnitude() > 40)
	{
		set_sprite_direction();
	} */
	
	set_sprite_direction_mouse();
	
	sprite_sheet->update_sprite_atlas(frame);
	
	MovementElement* me = get_element<MovementElement>();
	set_map_location(me->apply_velocity(get_map_location(), delta_time, z));
	me->apply_gravity(z, delta_time);
}

void DebugUnit::bind_inputs()
{
	std::function<void()> move_up = 		[this]() 
	{
		MovementElement* movement_element = get_element<MovementElement>();
		movement_element->movement_input = movement_element->movement_input + coordinates<float>(0.0f, 1.0f);
	};
		
	std::function<void()> move_right = 	[this]() 
	{
		MovementElement* movement_element = get_element<MovementElement>();
		movement_element->movement_input = movement_element->movement_input + coordinates<float>(1.0f, 0.0f);
	};
	
	
	std::function<void()> move_down = 	[this]() 
	{
		MovementElement* movement_element = get_element<MovementElement>();
		movement_element->movement_input = movement_element->movement_input + coordinates<float>(0.0f, -1.0f);
	};
	
	std::function<void()> move_left = 	[this]() 
	{
		MovementElement* movement_element = get_element<MovementElement>();
		movement_element->movement_input = movement_element->movement_input + coordinates<float>(-1.0f, 0.0f);
	};
	
	std::function<void()> primary =		[this]()
	{
		coordinates<float> target_location = get_map_location();
		Target target = Target(nullptr, target_location);
		this->debug_device->use(Command::Primary, this, target);
	};
	
	std::function<void()> tertiary =	[this]()
	{
		coordinates<float> target_location = get_map_location();
		Target target = Target(nullptr, target_location);
		this->debug_device->use(Command::Tertiary, this, target);
	};
	
	std::function<void()> tertiary_release =	[this]()
	{
		coordinates<float> target_location = get_map_location();
		Target target = Target(nullptr, target_location);
		this->debug_device->use(Command::StopTertiary, this, target);
	};
	
	std::function<void()> reload =	[this]()
	{
		this->debug_device->reload(this);
	};
	
	std::vector<InputBinding> binds =
	{
		{"move_up", input::KeyEventType::key_down, 0, move_up},
		{"move_right", input::KeyEventType::key_down, 0, move_right},
		{"move_down", input::KeyEventType::key_down, 0, move_down},
		{"move_left", input::KeyEventType::key_down, 0, move_left},
		{"primary", input::KeyEventType::key_down, 0, primary},
		{"tertiary", input::KeyEventType::key_down, 0, tertiary},
		{"tertiary", input::KeyEventType::key_up, 0, tertiary_release},
		{"reload", input::KeyEventType::key_down, 0, reload}
	};
	
	for (auto bind : binds)
	{
		input::AddInputBinding(bind);
	}
}

void DebugUnit::on_spawn()
{
	sprite = ptr::keeper<Sprite>(new Sprite(sprite_name));
	sprite_sheet = ptr::keeper<SpriteSheet>(new SpriteSheet(sprite_name, sprite));
	sprite->draw_size = {96, 192};
	
	get_engine()->spawn_entity<DebugDevice>(debug_device);
	get_engine()->get_entity(id, debug_device->owner, true);
	debug_device->draw_ui = true;
	
	// Helpers::create_floating_text("Spawned...", 16.0f, get_map_location(), 2.0f, {0, 0}, RenderGroup::Debug, 0.0f);
}

void DebugUnit::set_sprite_direction()
{
    frame = to_sheet_frame(get_element<MovementElement>()->velocity.isometric().normalize());
}

void DebugUnit::set_sprite_direction_mouse()
{
	coordinates<float> vec = ForgetteDirectX::world_to_screen(get_map_location());
	vec = vec.towards(input::get_cursor_screen_location()).normalize();
	
	// coordinates<float> vec = get_map_location().towards(input::get_cursor_map_location()).isometric().normalize();
	frame = to_sheet_frame(vec);
}

void DebugUnit::on_death()
{
	
}

int DebugUnit::to_sheet_frame(coordinates<float> vec)
{
	if (!vec)
	{
		return frame;
	}
	
	double angle = std::atan2(vec.y, vec.x);
	angle = core_math::normalize_angle(angle + (core_math::pi<float>/8));
	double segment = core_math::pi<float> / 4.0f;
	int segment_number = static_cast<int>(angle/segment);
	
	switch (segment_number)
	{
		case 0:
			return 7;
			break;
		case 1:
			return 8;
			break;
		case 2:
			return 1;
			break;
		case 3:
			return 2;
			break;
		case 4:
			return 3;
			break;
		case 5:
			return 4;
			break;
		case 6:
			return 5;
			break;
		case 7:
			return 6;
			break;
		default:
			return frame;
			break;
	}
}