module;
export module floating_text;
import directx;
import std;
import core;
import forgette;
import unit;

export
{
	class FloatingText : public Unit
	{
	public:
		FloatingText();
		
		virtual void game_update(float delta_time) override;
		
		std::string text;
		float life_time = 1.0f;
		coordinates<float> constant_input;
	};
	
	void create_floating_text(std::string text, float size, coordinates<float> map_location, float life_time, coordinates<float> input = {0.0f, 0.0f})
	{
		ptr::watcher<FloatingText> floating_text;
		get_engine()->spawn_entity<FloatingText>(map_location, floating_text);
		
		if (FloatingText* ft_ptr = floating_text.get())
		{
			floating_text->text = text;
			floating_text->life_time = life_time;
			floating_text->constant_input = input;
			
			get_engine()->render_functions[0].emplace_back([floating_text, size](float delta_time) mutable
			{
				if (!floating_text.get())
				{
					return false;
				}
				
				coordinates<float> screen_location = ForgetteDirectX::world_to_screen(floating_text->get_map_location());
				ForgetteDirectX::draw_text(floating_text->text, screen_location, size);
				return floating_text->life_time > 0.1f;
			});
		}
	}
}

void FloatingText::game_update(float delta_time)
{
	life_time -= delta_time;
	if (life_time <= 0)
	{
		pending_deletion = true;
	}
	
	movement.movement_input = constant_input;
	set_map_location(movement.apply_velocity(get_map_location(), delta_time));
}

FloatingText::FloatingText()
{
	display_name = "Floating Text";
	should_game_update = true;
	collision_enabled = false;
}