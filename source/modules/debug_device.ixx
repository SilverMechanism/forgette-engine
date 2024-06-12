module;
export module debug_device;

import device;

export
{
	class DebugDevice : public Device
	{
	public:
		DebugDevice();
		
		float pulse_timer = 0.0f;
		
		virtual void game_update(float delta_time) override;
	};
}