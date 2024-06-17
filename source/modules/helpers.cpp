module;
#include "defines.h"
module helpers;

import core;
import std;
import floating_text;
import magic_text;
import forgette;
import collision_element;
import entity;
import unit;

void Helpers::create_magic_text(std::string text, float size, coordinates<float> map_location, float life_time, coordinates<float> input, RenderGroup render_group, float z, std::vector<std::int64_t> ignored_entities)
{
	ptr::watcher<MagicText> floating_text;
	get_engine()->spawn_entity<MagicText>(map_location, floating_text);
	
	if (MagicText* ft_ptr = floating_text.get())
	{
		floating_text->text = text;
		floating_text->life_time = life_time;
		floating_text->size = size;
		floating_text->render_group = render_group;
		floating_text->drift = input;
		floating_text->z = z;
		floating_text->get_element<CollisionElement>()->ignored_entities = ignored_entities;
		floating_text->color = {0.5f, 0.9f, 0.9f, 1.0f};
	}
}

void Helpers::create_floating_text(std::string text, float size, coordinates<float> map_location, float life_time, coordinates<float> input, RenderGroup render_group, float z)
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

std::vector<ptr::watcher<Entity>> Helpers::try_overlap(CollisionGroup collision_group, std::set<CollisionGroup> collides_with, coordinates<float> location, float radius, std::vector<int64> to_ignore)
{
	std::vector<ptr::watcher<Entity>> overlaps;
	
	// !!! SLOW !!!
	// We need to start looking at optimizing this soon
	// through the planned sector system
	for (auto& entity : get_engine()->entities)
	{
		if (CollisionElement* other_ce = entity->get_element<CollisionElement>())
		{
			if (!other_ce)
			{
				continue;
			}
			
			Unit& unit1 = *dynamic_cast<Unit*>(other_ce->get_owner());
			
			bool ignore = false;
			for (auto id_to_ignore : to_ignore)
			{
				std::cout << "To ignore: " << id_to_ignore << std::endl;
				std::cout << "Unit ID: " << unit1.id << std::endl;
				if (unit1.id == id_to_ignore)
				{
					ignore = true;
					break;
				}
			}
			
			if (ignore)
			{
				continue;
			}
			
		    coordinates<float> pos1 = location;
		    coordinates<float> pos2 = unit1.get_map_location();
		
		    float dx = pos1.x - pos2.x;
		    float dy = pos1.y - pos2.y;
		    float distance = std::sqrt(dx * dx + dy * dy);
		    float collision_distance = other_ce->radius + radius;
		
		  	if (distance < collision_distance)
		  	{
		  		if (!collides_with.count(other_ce->collision_group) 
		    		|| !other_ce->collides_with.count(collision_group))
		    	{
		    		continue;
		    	}
				
				std::cout << "Adding " << entity->id << " to list." << std::endl;
				overlaps.push_back(ptr::watcher<Entity>(entity));
		  	}
		}
	}
	
	return overlaps;
}