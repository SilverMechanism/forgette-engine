module;
export module sector;

import std;
import entity;
import core;
import forgette;

export
{
	class Sector
	{
	public:
		Sector();
		
		std::vector<ptr::watcher<Entity>> entities;
		
		void analyze();
		
		void remove_entity(Entity* to_remove);
		
		void add_entity(const ptr::keeper<Entity>& to_add);
		// void add_entity(const ptr::watcher<Entity>& to_add);
		void add_entity(std::int64_t entity_id);
		
		void print_entity_names();
	};
}

Sector::Sector()
{
	
}

void Sector::remove_entity(Entity* to_remove)
{
    bool removed = false;
    
    // Perform the removal and set the flag if an entity is removed
    entities.erase(
        std::remove_if(
            entities.begin(),
            entities.end(),
            [to_remove, &removed](const ptr::watcher<Entity>& entity)
            {
                if (entity.get() == to_remove)
                {
                    removed = true;
                    return true;
                }
                return false;
            }
        ),
        entities.end());

    // Check the flag
    if (removed)
    {
        // std::cout << "Entity successfully removed." << std::endl;
    }
}

void Sector::add_entity(std::int64_t id)
{
	ptr::watcher<Entity> new_entity;
	get_engine()->get_entity(id, new_entity);
	if (new_entity.get())
	{
		// std::cout << "Added new entity " << new_entity->get_display_name() << std::endl;
		entities.push_back(std::move(new_entity));
	}
}

void Sector::add_entity(const ptr::keeper<Entity>& to_add)
{
	ptr::watcher<Entity> new_entity = ptr::watcher<Entity>(to_add);
	entities.push_back(std::move(new_entity));
}

/* void Sector::add_entity(const ptr::watcher<Entity>& to_add)
{

} */

void Sector::print_entity_names()
{
	std::cout << std::endl;
	std::cout << "=========" << std::endl;
	for (const auto& entity : entities)
	{
		std::cout << entity.get()->get_display_name();
		std::cout << std::endl;
	}
	std::cout << "=========" << std::endl;
	std::cout << std::endl;
}

void Sector::analyze()
{
	if (entities.size() < 2)
	{
		return;
	}
	
	for (const ptr::watcher<Entity>& entity : entities)
	{
		if (!entity.get())
		{
			std::cout << "invalid entity found in sector" << std::endl;
			continue;
		}
		if (entity.get()->pending_deletion)
		{
			std::cout << "entity pending deletion found in sector" << std::endl;
			continue;
		}
		
		std::cout << "Valid entity " << entity.get()->get_display_name() << " in sector." << std::endl;
	}
}