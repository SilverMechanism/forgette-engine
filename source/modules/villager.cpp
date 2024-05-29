/* module;
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
	class Villager : public DebugUnit, public InputHandler
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
	display_name = "Villager";
	sprite_name = "nona";

	should_game_update = true;
}

void DebugUnit::game_update(float delta_time)
{
	Unit::game_update(delta_time);
	
	set_map_location(movement.apply_velocity(get_map_location(), delta_time));
	
	std::cout << "[VILLAGER] ID: " << id << "\nPOS: " << std::string(get_map_location()) << "\n\n";
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
}*/