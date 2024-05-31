module;
export module spirit;
import std;
import core;
import unit;
import movement;
import timers;

export
{
	class Spirit
	{
	public:
		ptr::watcher<Unit> vessel;
		
		float target_delta;
		ptr::watcher<Unit> target;
		
		coordinates<float> target_pos;
		
		void update();
		bool enabled = true;
		bool target_active = false;
		
		bool wandering = false;
		bool resting = false;
		
		int th_rest = -1;
		
		void possess(ptr::keeper<Unit> new_unit);
		void possess(ptr::watcher<Unit> new_unit);
	private:
		bool should_retreat();
		void retreat();
		
		bool should_wander();
		void wander();
		
		bool should_attack_target();
		void attack_target();
	};
}

void Spirit::update()
{
	if (!vessel.get())
	{
		enabled = false;
		return;
	}
	
	if (!target.get())
	{
		// Look for target...
	}
	
	// target_pos = target.get()->get_map_location();
	// coordinates<float> vessel_pos = vessel.get()->get_map_location();
	
	// float target_delta = static_cast<float>(std::sqrt(std::pow(target_pos.x - vessel_pos.x, 2) + std::pow(target_pos.y - vessel_pos.y, 2)));
	
	if (should_wander())
	{
		wander();
	}
}

bool Spirit::should_retreat()
{
	if (vessel.get() && vessel.get()->health < 25.0f)
	{
		return true;
	}
	
	return false;
}

void Spirit::attack_target()
{
	std::cout << "[SPIRIT] I want to attack." << std::endl;
}

bool Spirit::should_wander()
{
	if (!vessel->movement.velocity)
	{
		wandering = false;
		
		if (resting)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void Spirit::wander()
{	
	vessel->movement.movement_input = {0, 0};
	
	if (resting)
	{
		return;
	}
	
	if (!target_active)
	{
		target_pos = vessel.get()->get_map_location().random_nearby(300.0f);
		target_active = true;
	}
	
	vessel->movement.movement_input = vessel->movement.movement_input + vessel->get_map_location().towards(target_pos);
	
	if ((vessel->get_map_location() - target_pos).magnitude() < 3.0f)
	{
		target_active = false;
		TimerManager* timer_manager = TimerManager::Instance();
		th_rest = timer_manager->CreateTimer(10.0f, [this](){resting = false;});
	}
}

void Spirit::possess(ptr::watcher<Unit> new_unit)
{
	if (new_unit.get())
	{
		vessel = ptr::watcher<Unit>(new_unit);
		enabled = true;
	}
}

void Spirit::possess(ptr::keeper<Unit> new_unit)
{
	if (new_unit.get())
	{
		vessel = ptr::watcher<Unit>(new_unit);
		enabled = true;
	}
}