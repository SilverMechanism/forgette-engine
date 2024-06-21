module unit;

import sprite;
import helpers;
import directx;
import game_map;
import forgette;
import sector;

Unit::Unit()
{
    add_entity_identifier(EntityClass::Unit);
    display_name = "Unit";
    should_render_update = true;
    type = EntityType::Unit;
}

void Unit::game_update(float delta_time)
{
    for (auto &unit : welded_units)
    {
        if (Unit* welded_unit = unit.get())
        {
            welded_unit->set_map_location(get_map_location());
        }
    }
}

void Unit::render_update(RenderGroup rg)
{
    if (rg == RenderGroup::Game)
    {
        if (Sprite* sprite_ptr = sprite.get())
        {
            sprite_ptr->render_to_map(get_map_location(), true, z);
        }
    }

    // std::cout << "Entity " << id << " using location for rendering: " << std::string(get_map_location()) << std::endl;
}

void Unit::set_sprite(std::string new_sprite_name)
{    
    /* if (!sprite.get())
    {
        Sprite* new_sprite = new Sprite(new_sprite_name);
        sprite = ptr::keeper<Sprite>(new_sprite);
    }
    else
    {
        sprite.get()->load(new_sprite_name);
    }

    sprite_name = new_sprite_name; */
}

std::string Unit::get_sprite_name()
{
    return sprite_name;
}

void Unit::on_spawn()
{
    // set_sprite(sprite_name);
}

coordinates<float> Unit::get_map_location() const
{
    return map_location;
}

void Unit::set_map_location(coordinates<float> new_location, bool no_check)
{
	if (no_check)
	{
		map_location = new_location;
		return;
	}
	
	if (!current_sector_info.sector)
	{
		if (GameMap* game_map = get_engine()->get_map())
        {
            // Get the sector info for the new location
            SectorInfo new_sector_info = game_map->get_sector_info(new_location);

            // Add the unit to the new sector
            new_sector_info.sector->add_entity(id);

            // Update the current sector info
            current_sector_info = new_sector_info;
        }
        
        // Update the unit's location
        map_location = new_location;
        last_location = new_location;
        
        return;
	}
	
    if (new_location != map_location)
    {
        // Calculate the distance between the new location and the center of the current sector
        float dx = new_location.x - current_sector_info.center.x;
        float dy = new_location.y - current_sector_info.center.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance > current_sector_info.size.x / 2.0f)
        {
            if (GameMap* game_map = get_engine()->get_map())
            {
                SectorInfo new_sector_info = game_map->get_sector_info(new_location);

                if (new_sector_info.sector != current_sector_info.sector)
                {
                    current_sector_info.sector->remove_entity(this);
					
					if (new_sector_info.sector)
					{
	                    new_sector_info.sector->add_entity(id);
	                    current_sector_info = new_sector_info;
					}
                }
            }
        }

        map_location = new_location;
        last_location = new_location;
    }
}
