module;
#include "defines.h"
export module villager;
import core;
import std;
import debug_unit;

export
{
	class Villager : public DebugUnit
	{
	public:
		Villager();
		
		virtual void bind_inputs(std::vector<InputBinding> inputs) override;
		
		virtual void game_update(float delta_time) override;
	private:
	};
}

Villager::Villager()
{
	display_name = "Villager";
	sprite_name = "nona";

	should_game_update = true;
}

void Villager::game_update(float delta_time)
{
	DebugUnit::game_update(delta_time);
}

void Villager::bind_inputs(std::vector<InputBinding> inputs)
{
	
}