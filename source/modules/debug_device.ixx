module;
export module debug_device;

import device;

class Unit;
class Action;
void set_text_movement(Unit* executor, Action* action);

export
{
	class DebugDevice : public Device
	{
	public:
		DebugDevice();
		~DebugDevice();
		
		double pulse_timer = 0.0f;
		
		virtual void game_update(float delta_time) override;
	};
}