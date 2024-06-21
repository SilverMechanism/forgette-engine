module;
export module movement_element;

import std;
import core;
import element;

export
{
	class MovementElement : public Element
	{
	public:
		coordinates<float> apply_velocity(coordinates<float> map_location, float delta_time, float z = 0.0f);
		void apply_gravity(float& z, float delta_time);
		
		coordinates<float> velocity {0, 0};
		coordinates<float> movement_input;
		coordinates<float> last_movement_input;
		
		float walk_speed = 64.0f;
		
		// Useful if controlled by a human to orient the movement input to the camera rotation
		bool skew_input = false;
	};
}