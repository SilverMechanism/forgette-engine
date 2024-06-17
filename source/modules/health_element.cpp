module;
#include <cassert>
module health_element;

import helpers;
import unit;

void HealthElement::damage(float amount, bool damage_text)
{
	hp_current -= amount;
	
	assert(owner);
	if (Unit* owning_unit = dynamic_cast<Unit*>(owner))
	{
		if (damage_text)
		{
			Helpers::create_floating_text(
				std::to_string(static_cast<int>(amount)), 
				16.0f, 
				owning_unit->get_map_location(), 
				1.0f, 
				coordinates<float>(0.0f, 100.0f).view_isometric(), 
				RenderGroup::Debug, 
				16.0f);
		}
		
		if (hp_current <= 0.0f)
		{
			owning_unit->pending_deletion = true;
		}
	}
	
	if (on_damage)
	{
		on_damage(amount);
	}
}

void HealthElement::heal(float amount, bool overheal)
{

}

float HealthElement::get_hp_percentage()
{
	return hp_current/hp_max;
}