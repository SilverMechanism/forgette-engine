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
		
		void possess_unit(ptr::watcher<Unit> unit);
		
		ptr::watcher<Unit> controlled_unit;
		std::vector<InputBinding> input_bindings;
		
		coordinates<float> view_rotation;
		
		Unit* get_controlled_unit();
	};
}

Player::Player()
{
	display_name = "Player";
	should_game_update = true;
	
	input_bindings.push_back({"primary", input::KeyEventType::key_down, 0});
	input_bindings.push_back({"secondary", input::KeyEventType::key_down, 0});
	input_bindings.push_back({"move_up", input::KeyEventType::key_down, 0});
	input_bindings.push_back({"move_right", input::KeyEventType::key_down, 0});
	input_bindings.push_back({"move_left", input::KeyEventType::key_down, 0});
	input_bindings.push_back({"move_down", input::KeyEventType::key_down, 0});
}

void Player::game_update(float delta_time)
{
	if (locked_to_unit && controlled_unit.get())
	{
		coordinates<float> render_pos = controlled_unit->get_map_location();
		coordinates<float> draw_size = controlled_unit->sprite->draw_size;
		// render_pos = {render_pos.x-draw_size.y/4, render_pos.y+draw_size.y/4};
		ForgetteDirectX::set_render_viewpoint(render_pos);
	}
}

void Player::possess_unit(ptr::watcher<Unit> unit)
{
	if (!unit.get())
	{
		std::cout << "[PLAYER] Cannot possess unit: invalid" << std::endl;
		return;
	}
	
	controlled_unit = ptr::watcher<Unit>(unit);
	
	if (InputHandler* ih = dynamic_cast<InputHandler*>(controlled_unit.get()))
	{
		ih->bind_inputs(input_bindings);
	}
	
	if (Unit* unit_unit = dynamic_cast<Unit*>(controlled_unit.get()))
	{
		unit_unit->movement.skew_input = true;
	}
	
	std::cout << "Attached to Entity " << controlled_unit.get()->id << std::endl;
}

Unit* Player::get_controlled_unit()
{
	return controlled_unit.get();
}