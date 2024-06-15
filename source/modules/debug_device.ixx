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
		
		std::uint8_t clip_current {8};
		std::uint8_t clip_maximum {8};
		
		virtual void game_update(float delta_time) override;
		virtual void render_update() override;
	};
}