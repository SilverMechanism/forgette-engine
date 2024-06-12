module;
#include "defines.h"
export module debug_unit;
import core;
import unit;
import input;
import std;
import movement;
import input_handler;
import sprite_sheet;
import directx;
import floating_text;

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
		
		virtual void bind_inputs(std::vector<InputBinding> inputs) override;
		
		virtual void game_update(float delta_time) override;
		
		std::unordered_map<std::string, std::function<void()>> binds;
		
		virtual void on_spawn() override;
		
		ptr::keeper<SpriteSheet> sprite_sheet;
		
		int frame = 0;
		
		int to_sheet_frame(coordinates<float> vec);
		
	protected:
		void set_sprite_direction();
		void set_sprite_direction_mouse();
		
	private:
	};
}

DebugUnit::DebugUnit()
{
	display_name = "Debug Unit";
	sprite_name = "waifu";

	should_game_update = true;
	
	// binds["primary"] = 		[this]() {std::cout << "Left mouse click!" << std::endl;};
	// binds["secondary"] = 	[this]() {std::cout << "Right mouse click!" << std::endl;};
	binds["move_up"] = 		[this]() 
	{
	this->movement.movement_input = this->movement.movement_input + coordinates<float>(0.0f, 1.0f);
	};
		
	binds["move_right"] = 	[this]() 
	{
		this->movement.movement_input = this->movement.movement_input + coordinates<float>(1.0f, 0.0f);
	};
	
	
	binds["move_down"] = 	[this]() 
	{
		this->movement.movement_input = this->movement.movement_input + coordinates<float>(0.0f, -1.0f);
	};
	
	binds["move_left"] = 	[this]() 
	{
		this->movement.movement_input = this->movement.movement_input + coordinates<float>(-1.0f, 0.0f);
	};
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
	
	set_map_location(movement.apply_velocity(get_map_location(), delta_time));
}

void DebugUnit::bind_inputs(std::vector<InputBinding> inputs)
{
	for (auto input : inputs)
	{
		if (binds.find(input.name) != binds.end())
		{
			input::AddInputBinding(input.name, input.key_event, binds[input.name], input.priority);
		}
	}
}

void DebugUnit::on_spawn()
{
	sprite = ptr::keeper<Sprite>(new Sprite(sprite_name));
	sprite_sheet = ptr::keeper<SpriteSheet>(new SpriteSheet(sprite_name, sprite));
	sprite->draw_size = {96, 192};
	
	FloatingText::create("Spawned...", 16.0f, get_map_location(), 5.0f);
}

void DebugUnit::set_sprite_direction()
{
    frame = to_sheet_frame(movement.velocity.isometric().normalize());
}

void DebugUnit::set_sprite_direction_mouse()
{
	coordinates<float> vec = ForgetteDirectX::world_to_screen(get_map_location());
	vec = vec.towards(input::get_cursor_screen_location()).normalize();
	
	// coordinates<float> vec = get_map_location().towards(input::get_cursor_map_location()).isometric().normalize();
	frame = to_sheet_frame(vec);
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