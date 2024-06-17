module;
export module player;

import input;
import entity;
import directx;
import input_handler;
import unit;
import core;
import health_element;

export
{
	class Player : public Entity
	{
	public:
		Player();
		
		void ZoomIn();
		void ZoomOut();
		
		virtual void game_update(float delta_time) override;
		virtual void render_update(RenderGroup rg) override;
		
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
	should_render_update = true;
	
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

void Player::render_update(RenderGroup rg)
{
	if (rg == RenderGroup::UI)
	{
		if (Unit* unit = controlled_unit.get())
		{
			ForgetteDirectX::draw_facing_helper(input::get_cursor_screen_location());
			
			if (HealthElement* health_element = unit->get_element<HealthElement>())
			{
				// draw hp
				coordinates<float> resolution = ForgetteDirectX::get_resolution();
				coordinates<float> draw_position = {resolution.x*0.06f, resolution.y*0.925f};
				
				std::string health_text = std::format("{}", static_cast<int>(health_element->get_hp_current()));
				
				float hp_percent = health_element->get_hp_percentage();
				float red = (hp_percent < 0.5f) ? 1.0f : 2.0f * (1.0f - hp_percent);
			    float green = (hp_percent < 0.5f) ? 2.0f * hp_percent : 1.0f;
			    float blue = 0.0f;
				ColorParams hp_color = ColorParams(red, green, blue, 1.0f);
				
				ForgetteDirectX::draw_text(health_text, draw_position, 72.0f, true, true, hp_color);
			}
		}
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
}

Unit* Player::get_controlled_unit()
{
	return controlled_unit.get();
}