module;
#include "defines.h"
export module debug_unit;
import core;
import unit;
import sprite_entity;
import forgette;
import input;
import std;

export
{
	SCRIPTABLE_UNIT(Debug)
	class DebugUnit : public Unit
	{
	public:
		DebugUnit();
		
		virtual void game_update(float delta_time) override;
	private:
		ptr::watcher<SpriteEntity> sprite;
	};
}

DebugUnit::DebugUnit()
{
	display_name = "Debug Unit";
	
	get_engine()->spawn_entity<SpriteEntity>(coordinates<float>(0,0), sprite);
	if (sprite)
	{
		sprite->reload_sprite(true, get_exe_dir() + L"sprites\\nona_small.png");
	}
	
	input::AddInputBinding(0x0044, input::KeyEventType::key_down, [this]() {this->set_map_location(get_map_location() + coordinates(1.0f, 0.0f));}, 0);
	input::AddInputBinding(0x0041, input::KeyEventType::key_down, [this]() {this->set_map_location(get_map_location() + coordinates(-1.0f, 0.0f));}, 0);
	input::AddInputBinding(0x0057, input::KeyEventType::key_down, [this]() {this->set_map_location(get_map_location() + coordinates(0.0f, 1.0f));}, 0);
	input::AddInputBinding(0x0053, input::KeyEventType::key_down, [this]() {this->set_map_location(get_map_location() + coordinates(0.0f, -1.0f));}, 0);

	should_game_update = true;
}

void DebugUnit::game_update(float delta_time)
{
	if (sprite)
	{
		sprite->set_map_location(get_map_location());
	}
}