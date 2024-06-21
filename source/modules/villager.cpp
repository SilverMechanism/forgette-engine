module;
#include "defines.h"
export module villager;

import core;
import std;
import debug_unit;
import game_sound;
import health_element;
import movement_element;

export
{
	class Villager : public DebugUnit
	{
	public:
		Villager();
		~Villager();
		
		virtual void bind_inputs() override;
		
		virtual void game_update(float delta_time) override;
		
		virtual void on_spawn() override;
		
		void on_damage(float amount);
		
		GameSound* hurt_sound = nullptr;
	private:
	};
}

Villager::Villager()
{
	add_entity_identifier(EntityClass::Unit);
	
	sprite_name = "chicken-beast";
	display_name = "Villager";

	should_game_update = true;
	
	hurt_sound = new GameSound("event:/Character/Chickenbeast/beast_hurt_01");
	
	add_element<HealthElement>();
	get_element<HealthElement>()->on_damage = [this](float amount) 
	{ 
		this->hurt_sound->play(get_map_location());
	};
	
	get_element<MovementElement>()->walk_speed = 32.0f;
}

Villager::~Villager()
{
	if (hurt_sound)
	{
		delete hurt_sound;
	}
}

void Villager::game_update(float delta_time)
{
	set_sprite_direction();
	
	sprite_sheet->update_sprite_atlas(frame);
	
	set_map_location(get_element<MovementElement>()->apply_velocity(get_map_location(), delta_time));
}

void Villager::bind_inputs()
{
	
}

void Villager::on_spawn()
{
	sprite = ptr::keeper<Sprite>(new Sprite(sprite_name));
	sprite_sheet = ptr::keeper<SpriteSheet>(new SpriteSheet(sprite_name, sprite));
	sprite->draw_size = {96, 192};
}

void Villager::on_damage(float amount)
{
	hurt_sound->play();
}