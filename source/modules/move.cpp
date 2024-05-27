module;
export module move;
import action;
import std;
import core;

export
{
	class Move : public Action
	{
	public:
		virtual void execute() override;
		
		coordinates<float> velocity;
		coordinates<float> movement_input;
		coordinates<float> last_movement_input;
		
	};
}

void Move::execute()
{
    // Update velocity by subtracting the last movement input
    velocity = {velocity.x - last_movement_input.x, velocity.y - last_movement_input.y};
    
    if (std::abs(velocity.x) > 0 || std::abs(velocity.y) > 0)
    {
        if (velocity.x > 0.0f)
        {
            velocity.x = std::max(0.0f, velocity.x - 1.0f);
        }
        else
        {
            velocity.x = std::min(0.0f, velocity.x + 1.0f);
        }
        
        if (velocity.y > 0.0f)
        {
            velocity.y = std::max(0.0f, velocity.y - 1.0f);
        }
        else
        {
            velocity.y = std::min(0.0f, velocity.y + 1.0f);
        }
    }
    
    if (movement_input)
    {
    	// Convert input to match the isometric projection
    	coordinates<float> base_input = movement_input;
    	movement_input.x = base_input.x - base_input.y;
    	movement_input.y = base_input.x + base_input.y;
    	
    	// Normalize the input
        float input_size = std::sqrt(movement_input.x * movement_input.x + movement_input.y * movement_input.y);
        if (input_size > 0.0f) {
            movement_input.x /= input_size;
            movement_input.y /= input_size;
        }
        
        velocity = {velocity.x + movement_input.x, velocity.y + movement_input.y};
    }
    
    last_movement_input = movement_input;
    movement_input = {0.0f, 0.0f}; // Reset movement input after applying it
    // std::cout << "Current velocity: " << std::string(velocity) << std::endl;
}