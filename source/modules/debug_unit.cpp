module;
#include "defines.h"
export module debug_unit;
import core;
import unit;
import sprite_entity;
import input;
import std;
import movement;
import input_handler;

export
{
	SCRIPTABLE_UNIT(Debug)
	class DebugUnit : public Unit, public InputHandler
	{
	public:
		DebugUnit();
		
		Movement movement;
		virtual void bind_inputs(std::vector<InputBinding> inputs) override;
		
		virtual void game_update(float delta_time) override;
		
		std::unordered_map<std::string, std::function<void()>> binds;
	private:
	};
}

DebugUnit::DebugUnit()
{
	display_name = "Debug Unit";
	sprite_name = "man-nw";

	should_game_update = true;
	
	// binds["primary"] = 		[this]() {std::cout << "Left mouse click!" << std::endl;};
	// binds["secondary"] = 	[this]() {std::cout << "Right mouse click!" << std::endl;};
	binds["move_up"] = 		[this]() {this->movement.movement_input = this->movement.movement_input + coordinates<float>(0.0f, 1.0f);};
	binds["move_right"] = 	[this]() {this->movement.movement_input = this->movement.movement_input + coordinates<float>(1.0f, 0.0f);};
	binds["move_down"] = 	[this]() {this->movement.movement_input = this->movement.movement_input + coordinates<float>(0.0f, -1.0f);};
	binds["move_left"] = 	[this]() {this->movement.movement_input = this->movement.movement_input + coordinates<float>(-1.0f, 0.0f);};
}

void DebugUnit::game_update(float delta_time)
{
	Unit::game_update(delta_time);
	
	set_map_location(movement.apply_velocity(get_map_location(), delta_time));
	
	// std::cout << "[DEBUG UNIT] ID: " << id << "\nPOS: " << std::string(get_map_location()) << "\n\n";
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