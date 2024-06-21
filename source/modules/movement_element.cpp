module;
module movement_element;

import std;
import core;
import game_map;

coordinates<float> MovementElement::apply_velocity(coordinates<float> map_location, float delta_time, float z)
{
	bool walking = core_math::nearly_zero(z);
	
    // Update velocity by subtracting the last movement input
    if (walking)
    {
    	velocity = {velocity.x - last_movement_input.x, velocity.y - last_movement_input.y};
    }
    
    float decay_step = 0.0f;
	if (walking)
	{
		decay_step = walk_speed;
	}
	
	velocity.x = core_math::interp_combo(velocity.x, 0.0f, 0.005f, decay_step);
	velocity.y = core_math::interp_combo(velocity.y, 0.0f, 0.005f, decay_step);
	
    if (walking && movement_input)
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

void MovementElement::apply_gravity(float& z, float delta_time)
{
	if (z > 0.0f)
	{
		z -= (gravity * delta_time);
	}
	if (z < 0.0f)
	{
		z = 0.0f;
	}
}