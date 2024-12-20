module;
module floating_text;

import directx;
import std;
import core;
import forgette;
import unit;
import movement_element;
import sprite;

void FloatingText::game_update(float delta_time)
{
	life_time -= delta_time;
	if (life_time <= 0)
	{
		pending_deletion = true;
	}
	
	// movement.velocity = drift;
	set_map_location(get_map_location()+ (drift * delta_time));
}

void FloatingText::render_update(RenderGroup rg)
{
	coordinates<float> screen_location = ForgetteDirectX::world_to_screen(get_map_location(), z);
	ForgetteDirectX::draw_text(
		text, 
		screen_location, 
		size, 
		true, 
		false, 
		color);
}

FloatingText::FloatingText()
{
	add_entity_identifier(EntityClass::FloatingText);
	
	display_name = "Floating Text";
	should_game_update = true;
	should_render_update = true;
	render_group = RenderGroup::Debug;
	
	add_element<MovementElement>();
}