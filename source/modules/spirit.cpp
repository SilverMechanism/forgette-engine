module;
export module spirit;
import std;
import core;
import unit;
import movement_element;
import timers;
import entity;
import sprite;

export
{
	class Spirit : public Entity
	{
	public:
		Spirit();
		
		ptr::watcher<Unit> vessel;
		
		float target_delta;
		ptr::watcher<Unit> target;
		
		coordinates<float> target_pos;
		
		virtual void game_update(float delta_time) override;
		bool enabled = true;
		bool target_active = false;
		
		bool wandering = false;
		bool resting = false;
		
		int th_rest = -1;
		
		void inhabit(ptr::keeper<Unit> new_unit);
		void inhabit(ptr::watcher<Unit> new_unit);
		
	private:
		bool should_retreat();
		void retreat();
		
		bool should_wander();
		void wander();
		
		bool should_attack_target();
		void attack_target();
	};
}

Spirit::Spirit()
{
	should_game_update = true;
}

void Spirit::game_update(float delta_time)
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
	/* if (vessel.get() && vessel.get()->health < 25.0f)
	{
		return true;
	} */
	
	return false;
}

void Spirit::attack_target()
{
	std::cout << "[SPIRIT] I want to attack." << std::endl;
}

bool Spirit::should_wander()
{
	if (!vessel->get_element<MovementElement>()->velocity)
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
	
	return true;
}

void Spirit::wander()
{
	if (resting)
	{
		return;
	}
	
	if (!target_active)
	{
		target_pos = vessel.get()->get_map_location().random_nearby(300.0f);
		target_active = true;
	}
	
	MovementElement* vessel_movement = vessel->get_element<MovementElement>();
	vessel_movement->movement_input = vessel_movement->movement_input + vessel->get_map_location().towards(target_pos);
	
	if ((vessel->get_map_location() - target_pos).magnitude() < 3.0f)
	{
		target_active = false;
		resting = true;
		TimerManager* timer_manager = TimerManager::Instance();
		th_rest = timer_manager->CreateTimer(3.0f, [this](int calls){resting = false;});
	}
}

void Spirit::inhabit(ptr::watcher<Unit> new_unit)
{
	if (new_unit.get())
	{
		vessel = ptr::watcher<Unit>(new_unit);
		enabled = true;
	}
}

void Spirit::inhabit(ptr::keeper<Unit> new_unit)
{
	if (new_unit.get())
	{
		vessel = ptr::watcher<Unit>(new_unit);
		enabled = true;
	}
}