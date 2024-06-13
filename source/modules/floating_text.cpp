module;
module floating_text;

import directx;
import std;
import core;
import forgette;
import unit;

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