module;
#include "defines.h"
export module debug_unit;
import core;
import unit;
import sprite_entity;
import forgette;
import input;
import std;
import movement;

export
{
	SCRIPTABLE_UNIT(Debug)
	class DebugUnit : public Unit
	{
	public:
		DebugUnit();
		
		Movement movement;
		
		virtual void game_update(float delta_time) override;
	private:
	};
}

DebugUnit::DebugUnit()
{
	display_name = "Debug Unit";
	sprite_name = "man-nw";
	
	input::AddInputBinding(0x0044, input::KeyEventType::key_down, [this]() {this->movement.movement_input = this->movement.movement_input + coordinates<float>(1.0f, 0.0f);}, 0);
	input::AddInputBinding(0x0041, input::KeyEventType::key_down, [this]() {this->movement.movement_input = this->movement.movement_input + coordinates<float>(-1.0f, 0.0f);}, 0);
	input::AddInputBinding(0x0057, input::KeyEventType::key_down, [this]() {this->movement.movement_input = this->movement.movement_input + coordinates<float>(0.0f, 1.0f);}, 0);
	input::AddInputBinding(0x0053, input::KeyEventType::key_down, [this]() {this->movement.movement_input = this->movement.movement_input + coordinates<float>(0.0f, -1.0f);}, 0);

	should_game_update = true;
}

void DebugUnit::game_update(float delta_time)
{
	Unit::game_update(delta_time);
	
	set_map_location(movement.apply_velocity(map_location, delta_time));
}