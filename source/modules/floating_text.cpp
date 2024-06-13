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
		virtual void render_update() override;
		
		coordinates<float> drift;
		std::string text;
		float life_time = 1.0f;
		float size = 16.0f;
	};
	
	void create_floating_text(std::string text, float size, coordinates<float> map_location, float life_time, coordinates<float> input = {0.0f, 0.0f}, RenderGroup render_group = RenderGroup::Debug, float z = 0.0f)
	{
		ptr::watcher<FloatingText> floating_text;
		get_engine()->spawn_entity<FloatingText>(map_location, floating_text);
		
		if (FloatingText* ft_ptr = floating_text.get())
		{
			floating_text->text = text;
			floating_text->life_time = life_time;
			floating_text->size = size;
			floating_text->render_group = render_group;
			floating_text->drift = input;
			floating_text->z = z;
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
	
	movement.velocity = drift;
	set_map_location(movement.apply_velocity(get_map_location(), delta_time));
}

void FloatingText::render_update()
{
	coordinates<float> screen_location = ForgetteDirectX::world_to_screen(get_map_location(), z);
	ForgetteDirectX::draw_text(text, screen_location, size);
}

FloatingText::FloatingText()
{
	display_name = "Floating Text";
	should_game_update = true;
	should_render_update = true;
	collision_enabled = false;
	render_group = RenderGroup::Debug;
}