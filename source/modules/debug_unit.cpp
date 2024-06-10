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

std::unordered_map<std::string, std::uint8_t> angles =
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
	private:
		void set_sprite_direction();
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
	
	if (sprite_sheet.get() && sprite.get() && movement.velocity.magnitude() > 40)
	{
		set_sprite_direction();
	}
	
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
}

void DebugUnit::set_sprite_direction()
{
    if (!(movement.movement_input.x == 0 && movement.movement_input.y == 0))
    {
	    constexpr double angle_threshold = core_math::tan(core_math::pi<float> / 8.0); // 22.5 degrees in radians
	
	    if (std::fabs(movement.movement_input.y) > std::fabs(movement.movement_input.x))
	    {
	        if (movement.movement_input.y > 0)
	        {
	            if (movement.movement_input.x > angle_threshold * movement.movement_input.y)
	            {
	                frame = angles["up_right"];
	            }
	            else if (movement.movement_input.x < -angle_threshold * movement.movement_input.y)
	            {
	                frame = angles["up_left"];
	            }
	            else
	            {
	                frame = angles["up"];
	            }
	        }
	        else
	        {
	            if (movement.movement_input.x > angle_threshold * -movement.movement_input.y)
	            {
	                frame = angles["down_right"];
	            }
	            else if (movement.movement_input.x < -angle_threshold * -movement.movement_input.y)
	            {
	                frame = angles["down_left"];
	            }
	            else
	            {
	                frame = angles["down"];
	            }
	        }
	    }
	    else
	    {
	        if (movement.movement_input.x > 0)
	        {
	            if (movement.movement_input.y > angle_threshold * movement.movement_input.x)
	            {
	                frame = angles["up_right"];
	            }
	            else if (movement.movement_input.y < -angle_threshold * movement.movement_input.x)
	            {
	                frame = angles["down_right"];
	            }
	            else
	            {
	                frame = angles["right"];
	            }
	        }
	        else
	        {
	            if (movement.movement_input.y > angle_threshold * -movement.movement_input.x)
	            {
	                frame = angles["up_left"];
	            }
	            else if (movement.movement_input.y < -angle_threshold * -movement.movement_input.x)
	            {
	                frame = angles["down_left"];
	            }
	            else
	            {
	                frame = angles["left"];
	            }
	        }
	    }
    }
	
    sprite_sheet->update_sprite_atlas(frame);
}