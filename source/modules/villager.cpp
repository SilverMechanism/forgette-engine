module;
#include "defines.h"
export module villager;
import core;
import std;
import debug_unit;

export
{
	class Villager : public DebugUnit
	{
	public:
		Villager();
		
		virtual void bind_inputs(std::vector<InputBinding> inputs) override;
		
		virtual void game_update(float delta_time) override;
		
		virtual void on_spawn() override;
	private:
	};
}

Villager::Villager()
{
	sprite_name = "chicken-beast";
	display_name = "Villager";

	should_game_update = true;
}

void Villager::game_update(float delta_time)
{
	set_sprite_direction();
	
	sprite_sheet->update_sprite_atlas(frame);
	
	set_map_location(movement.apply_velocity(get_map_location(), delta_time));
}

void Villager::bind_inputs(std::vector<InputBinding> inputs)
{
	
}

void Villager::on_spawn()
{
	sprite = ptr::keeper<Sprite>(new Sprite(sprite_name));
	sprite_sheet = ptr::keeper<SpriteSheet>(new SpriteSheet(sprite_name, sprite));
	sprite->draw_size = {96, 192};
}