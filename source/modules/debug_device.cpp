module;
module debug_device;

import debug_action;
import input;
import helpers;

DebugDevice::DebugDevice()
{
	should_game_update = true;
}

DebugDevice::~DebugDevice()
{
	
}

void DebugDevice::game_update(float delta_time)
{
	
}

void DebugDevice::render_update()
{

}

template<>
void DebugDevice::use<coordinates<float>>(Unit* user, coordinates<float>& target)
{
	Helpers::create_magic_text(
		"Pew!", 
		16.0f,
		target,
		6.0f,
		user->get_map_location().towards(input::get_cursor_map_location()) * 233.0f,
		RenderGroup::Game,
		48.0f,
		{user->id});
}