module;
module debug_device;

DebugDevice::DebugDevice()
{
	should_game_update = true;
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