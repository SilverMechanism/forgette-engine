module;
export module debug_device;

import device;
import game_sound;

struct Target;
class Unit;

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
		virtual void render_update(RenderGroup rg) override;
		
		void fire_text(Unit* user, Target& target);
		
		float reload_time = 1.5f;
		
		inline void reload(Unit* user);
		
		GameSound* primary_sound = nullptr;
		GameSound* secondary_sound = nullptr;
		
		void discharge(Unit* user, Target& target);
		void do_discharge(Unit* user, coordinates<float> location);
		
	private:
		bool reloading = false;
	};
}