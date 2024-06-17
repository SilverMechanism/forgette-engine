module;
export module health_element;

import element;
import std;

export
{
	class HealthElement : public Element
	{
	protected:
		float hp_max_base = 100.0f;
		float hp_max = 100.0f;
		float hp_current = 100.0f;
		
	public:
		void damage(float amount, bool damage_text = true);
		void heal(float amount, bool overheal);
		
		std::function<void(float amount)> on_damage;
		
		float get_hp_percentage();
		
		float get_hp_current()
		{
			return hp_current;
		}
		float get_hp_max()
		{
			return hp_max;
		}
		float get_hp_max_base()
		{
			return hp_max_base;
		}
	};
}