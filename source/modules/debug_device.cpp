module;
module debug_device;

import debug_action;
import input;

DebugDevice::DebugDevice()
{
	action = new DebugAction();
	process_executor = set_text_movement;
	should_game_update = true;
}

DebugDevice::~DebugDevice()
{
	if (action)
	{
		delete action;
	}
}

void set_text_movement(Unit* executor, Action* action)
{
	static_cast<DebugAction*>(action)->text_drift = executor->get_map_location().towards(input::get_cursor_map_location()).normalize();
}

void DebugDevice::game_update(float delta_time)
{
	pulse_timer += delta_time;
	
	if (pulse_timer >= 2.0f)
	{
		std::cout << "Pulsing!" << std::endl;
		pulse_timer = 0.0f;
	}
}