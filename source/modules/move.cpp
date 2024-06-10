module;
export module movement;
import action;
import std;
import core;

export
{
	class Movement
	{
	public:
		coordinates<float> apply_velocity(coordinates<float> map_location, float delta_time);
		
		coordinates<float> velocity {0, 0};
		coordinates<float> movement_input;
		coordinates<float> last_movement_input;
		
		float walk_speed = 64.0f;
		
		// Useful if controlled by a human to orient the movement input to the camera rotation
		bool skew_input = false;
	};
}

coordinates<float> Movement::apply_velocity(coordinates<float> map_location, float delta_time)
{
    // Update velocity by subtracting the last movement input
    velocity = {velocity.x - last_movement_input.x, velocity.y - last_movement_input.y};
    
    if (std::abs(velocity.x) > 0 || std::abs(velocity.y) > 0)
    {
        if (velocity.x > 0.0f)
        {
            velocity.x = std::max(0.0f, velocity.x - walk_speed);
        }
        else
        {
            velocity.x = std::min(0.0f, velocity.x + walk_speed);
        }
        
        if (velocity.y > 0.0f)
        {
            velocity.y = std::max(0.0f, velocity.y - walk_speed);
        }
        else
        {
            velocity.y = std::min(0.0f, velocity.y + walk_speed);
        }
    }
    
    if (movement_input)
    {
    	if (skew_input)
    	{
	    	movement_input = movement_input.view_isometric();
    	}
    	
    	// Normalize the input
        movement_input = movement_input.normalize();
        
        velocity = {velocity.x + (movement_input.x*walk_speed), velocity.y + (movement_input.y*walk_speed)};
    }
    
    last_movement_input = movement_input;
    movement_input = {0.0f, 0.0f}; // Reset movement input after applying it
    // std::cout << "Current velocity: " << std::string(velocity) << std::endl;
    
    return map_location + (velocity * delta_time);
}