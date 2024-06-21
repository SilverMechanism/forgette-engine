module;
export module debug_device;

import device;
import game_sound;

struct Target;
export class Unit;

class GameSound;

export
{
	class DebugDevice : public Device
	{
	public:
		DebugDevice();
		~DebugDevice();
		
		double pulse_timer = 0.0f;
		
		std::uint8_t clip_current {3};
		std::uint8_t clip_maximum {3};
		
		virtual void game_update(float delta_time) override;
		virtual void render_update(RenderGroup rg) override;
		
		void fire_text(Unit* user, Target& target);
		
		float reload_time = 1.5f;
		
		void reload(const Unit* user);
		
		GameSound* primary_sound = nullptr;
		GameSound* secondary_sound = nullptr;
		
		void charge(Unit* user);
		void stop_charge();
		
		void do_overload();
		
		bool overload_on_cooldown = false;
		
		float overload_power = 0.0f;
		float overload_charge_rate = 4.0f;
		float overload_cooldown = 0.5f;
		float overload_power_maximum = 2.0f;
		float overload_aoe = 55.0f;
		float overload_damage = 30.0f;
		
		bool charging = false;
		bool draw_ui = false;
	private:
		bool reloading = false;
	};
}