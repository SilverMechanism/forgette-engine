module;
export module player;
import input;
import entity;
import directx;
import input_handler;
import unit;
import core;

export
{
	class Player : public Entity
	{
	public:
		Player();
		
		void ZoomIn();
		void ZoomOut();
		
		virtual void game_update(float delta_time) override;
		
		bool locked_to_unit = true;
		
		void possess_unit(ptr::watcher<Entity> unit);
		
		ptr::watcher<Entity> controlled_unit;
		std::vector<InputBinding> input_bindings;
		
		coordinates<float> view_rotation;
	};
}

Player::Player()
{
	display_name = "Player";
	should_game_update = true;
	
	input_bindings.push_back({"primary", input::KeyEventType::key_down, 0});
	input_bindings.push_back({"secondary", input::KeyEventType::key_down, 0});
	input_bindings.push_back({"move_right", input::KeyEventType::key_down, 0});
	input_bindings.push_back({"move_left", input::KeyEventType::key_down, 0});
	input_bindings.push_back({"move_down", input::KeyEventType::key_down, 0});
}

void Player::game_update(float delta_time)
{
	if (locked_to_unit && controlled_unit.get())
	{
		set_map_location(controlled_unit->get_map_location());
		// std::cout << "[PLAYER] Controlled unit ID: " << controlled_unit->id << "\nControlled unit POS: " << std::string(controlled_unit->get_map_location()) << "\n\n";
	}
	else
	{
		// std::cout << "No controlled unit" << std::endl;
	}
}

void Player::possess_unit(ptr::watcher<Entity> unit)
{
	if (!unit.get())
	{
		std::cout << "[PLAYER] Cannot possess unit: invalid" << std::endl;
		return;
	}
	
	controlled_unit = ptr::watcher<Entity>(unit);
	
	if (InputHandler* ih = dynamic_cast<InputHandler*>(controlled_unit.get()))
	{
		ih->bind_inputs(input_bindings);
	}
	
	std::cout << "Attached to Entity " << controlled_unit.get()->id << std::endl;
}