module;
#include <windows.h>
#include <assert.h>
module forgette;

import gfx;
import std;
import windows;
import timers;
import entity;
import player;
import input;
import core;
import directx;
import lua_manager;
import movement_element;
import unit;
import game_map;
import sound;
import collision_element;
import sector;

std::unique_ptr<Forgette::Engine> global_engine = nullptr;

LuaManager* Forgette::Engine::get_lua_manager()
{
	return lua_manager.get();
}

Player* Forgette::Engine::create_player()
{
	spawn_entity<Player>(coordinates(0,0), local_player);
	return local_player.get();
}

Forgette::Engine* get_engine()
{
	return global_engine.get();
}

GameMap* Forgette::Engine::get_map()
{
	return active_map.get();
}

void Forgette::Engine::add_unit_to_sector(coordinates<float> map_location, const ptr::keeper<Entity>& keeper)
{
	Sector* sector = active_map->get_sector(map_location);
	sector->add_entity(keeper);
}

void Forgette::Engine::add_unit_to_sector(coordinates<float> map_location, std::int64_t entity_id)
{
	Sector* sector = active_map->get_sector(map_location);
	sector->add_entity(entity_id);
}

void set_engine(Forgette::Engine* engine)
{
	if (global_engine.get() != nullptr)
	{
		assert("Attempted to set global engine reference when it already existed");
	}
	global_engine = std::unique_ptr<Forgette::Engine>(engine);
}

namespace Forgette
{
	Engine::Engine()
	{
		if (!GFX::initialize())
		{
			return;
		}

		timer_manager = TimerManager::Instance();
		timer_manager->Initialize();
		
		loop_functions[Mode::not_set] = std::bind(&Engine::setup_map, this);
		loop_functions[Mode::in_map] = std::bind(&Engine::map_loop, this);
		
		LuaManager* new_lua_manager = new LuaManager;
		lua_manager = ptr::keeper<LuaManager>(new_lua_manager);
		
		input::map_key("primary", 0x0001, true);
		input::map_key("secondary", 0x0002, true);
		input::map_key("tertiary", 0x0020, true);
		input::map_key("move_up", 0x0057, true);
		input::map_key("move_right", 0x0044, true);
		input::map_key("move_down", 0x0053, true);
		input::map_key("move_left", 0x0041, true);
		input::map_key("reload", 0x0052, true);
		
		Sound::initialize();
		
		std::cout << 
		R"(

 ______   ______     ______     ______     ______     ______   ______   ______       
/\  ___\ /\  __ \   /\  == \   /\  ___\   /\  ___\   /\__  _\ /\__  _\ /\  ___\      
\ \  __\ \ \ \/\ \  \ \  __<   \ \ \__ \  \ \  __\   \/_/\ \/ \/_/\ \/ \ \  __\      
 \ \_\    \ \_____\  \ \_\ \_\  \ \_____\  \ \_____\    \ \_\    \ \_\  \ \_____\    
  \/_/     \/_____/   \/_/ /_/   \/_____/   \/_____/     \/_/     \/_/   \/_____/    
                                                                                     
 ______     __   __     ______     __     __   __     ______                         
/\  ___\   /\ "-.\ \   /\  ___\   /\ \   /\ "-.\ \   /\  ___\                        
\ \  __\   \ \ \-.  \  \ \ \__ \  \ \ \  \ \ \-.  \  \ \  __\                        
 \ \_____\  \ \_\\"\_\  \ \_____\  \ \_\  \ \_\\"\_\  \ \_____\                      
  \/_____/   \/_/ \/_/   \/_____/   \/_/   \/_/ \/_/   \/_____/    


)";
	}

	Engine::~Engine()
	{

	}

	void Engine::load_game_map(std::wstring script_path)
	{
		active_mode = Mode::in_map;
		
		GameMap* new_map = new GameMap
		(
			coordinates<float>(64.0f, 64.0f), 
			"JAFM", 
			coordinates<std::uint16_t>(50, 50)
		);
		
		active_map = ptr::keeper<GameMap>(new_map);
		active_map.get()->generate_flatmap("e", 128, 128);
		
		lua_manager.get()->run_lua_script(script_path);
		
		return;
	}

	void Engine::loop()
	{
		ForgetteDirectX::prerender();
		
		timer_manager->ParseTimers();
		// timer_manager->ProcessCallbacks();

		for (const auto& pair : input::active_down_binds)
		{
			pair.second.key_down_bindings[0]();
		}
		
		loop_functions[active_mode]();
		
		Sound::studioSystem->update();

		ForgetteDirectX::present(true);
	}
	
	bool z_compare(const ptr::keeper<Entity> &a, const ptr::keeper<Entity> &b)
	{
		
	    Unit* unit1 = nullptr;
	    Unit* unit2 = nullptr;
	    
	    if (a.get()->is_entity(EntityClass::Unit))
	    {
	    	unit1 = static_cast<Unit*>(a.get());
	    }
	    if (b.get()->is_entity(EntityClass::Unit))
	    {
	    	unit2 = static_cast<Unit*>(b.get());
	    }
	
	    // If either entity is not a unit, they should not affect the sorting.
	    if (!unit1) return false;
	    if (!unit2) return true;
	    
	    if (unit1->render_group != unit2->render_group)
	    {
	        return (unit1->render_group < unit2->render_group);
	    }
	
	    auto map_location_a = unit1->get_map_location().isometric();
	    auto map_location_b = unit2->get_map_location().isometric();
	
	    if (map_location_a.y == map_location_b.y)
	    {
	        return map_location_a.x < map_location_b.x;
	    }
	
	    return map_location_a.y < map_location_b.y;
	}
	
	void handle_collision(CollisionElement& ce1, CollisionElement& ce2, float delta_time)
	{
		
		if (!ce1.get_owner()->is_entity(EntityClass::Unit) || !ce2.get_owner()->is_entity(EntityClass::Unit))
		{
			return;
		}
		
		Unit& unit1 = *static_cast<Unit*>(ce1.get_owner());
		Unit& unit2 = *static_cast<Unit*>(ce2.get_owner());
		
		if (!ce1.collision_enabled || !ce2.collision_enabled)
		{
			return;
		}
		
		if (unit1.z > ce2.height || unit2.z > ce1.height)
		{
			return;
		}
		
	    coordinates<float> pos1 = unit1.get_map_location();
	    coordinates<float> pos2 = unit2.get_map_location();
	    
	    float dx = pos1.x - pos2.x;
	    float dy = pos1.y - pos2.y;
	    float distance = std::sqrt(dx * dx + dy * dy);
	    float collision_distance = ce1.radius + ce2.radius;
	
	    if (distance < collision_distance)
	    {
	    	if (!ce1.collides_with.count(ce2.collision_group) 
	    		|| !ce2.collides_with.count(ce1.collision_group))
	    	{
	    		return;
	    	}
	    	
			const std::vector<std::int64_t>& ents1 = ce1.ignored_entities;
			const std::vector<std::int64_t>& ents2 = ce2.ignored_entities;
			
			if (std::find(ents1.begin(), ents1.end(), unit2.id) != ents1.end()
				|| std::find(ents2.begin(), ents2.end(), unit1.id) != ents2.end())
			{
				return;
			}
			
			if (ce1.on_collision)
			{
				ce1.on_collision(&unit2);
			}
			if (ce2.on_collision)
			{
				ce2.on_collision(&unit1);
			}
			
	        // Calculate the amount to rewind
	        float overlap = collision_distance - distance;
	        
	        // Normalize the direction vector
	        float nx = dx / distance;
	        float ny = dy / distance;
			
			MovementElement* unit1_movement = unit1.get_element<MovementElement>();
			MovementElement* unit2_movement = unit2.get_element<MovementElement>();
			
	        // Calculate the total velocities along the collision normal
	        float v1_dot_n = unit1_movement->velocity.x * nx + unit1_movement->velocity.y * ny;
	        float v2_dot_n = unit2_movement->velocity.x * nx + unit2_movement->velocity.y * ny;
	
	        // Calculate how much to rewind each unit based on their velocities and delta_time
	        float rewind_factor1 = v1_dot_n * delta_time;
	        float rewind_factor2 = v2_dot_n * delta_time;
	
	        // Adjust the positions by the smallest possible amount to stop the collision
	        pos1.x -= rewind_factor1 * nx;
	        pos1.y -= rewind_factor1 * ny;
	        pos2.x -= rewind_factor2 * nx;
	        pos2.y -= rewind_factor2 * ny;
			
			if (unit1_movement->velocity)
			{
				unit1.set_map_location(pos1, true);
			}
	        if (unit2_movement->velocity)
	        {
	        	unit2.set_map_location(pos2, true);
	        }
	    }
	}
	
	void Engine::setup_map()
	{
		std::wstring map_script_path = get_application_dir() + L"\\blood_fields.tofs";
		load_game_map(map_script_path);
	}
	
	void Engine::map_loop()
	{
		if (!new_spawns.empty())
		{
			for (auto& entity : new_spawns)
			{
				if (entity.get()->is_entity(EntityClass::Unit))
				{
					Unit* unit = static_cast<Unit*>(entity.get());
					add_unit_to_sector(unit->get_map_location(), entity);
				}
			}
			
			entities.insert(
				entities.end(),
				std::make_move_iterator(new_spawns.begin()),
				std::make_move_iterator(new_spawns.end()));

			new_spawns.clear();
		}
	    
		const float delta_time = timer_manager->get_delta_time();
		
		if (!active_map.get())
		{
			return;
		}
		
		for (auto it = entities.begin(); it != entities.end(); /* no increment here */) 
		{
	        if (it->get()->pending_deletion) 
	        {
	        	if (it->get()->is_entity(EntityClass::Unit))
	        	{
	        		Unit* unit = static_cast<Unit*>(it->get());
	        		if (unit->current_sector_info.sector)
		        	{
		        		unit->current_sector_info.sector->remove_entity(it->get());
		        	}
	        	}
	        	
	            it->~keeper();
	            it = entities.erase(it);
	        } 
	        else 
	        {
	            ++it;
	        }
	    }
		
		active_map.get()->render_tiles();
		
		std::sort(entities.begin(), entities.end(), z_compare);
		
		for (auto& entity : entities)
		{
			if (entity.get()->pending_deletion)
			{
				continue;
			}
			
			if (entity.get()->should_game_update) 
			{
				entity.get()->game_update(delta_time);
			}
			else
			{
				// std::println("Did not run game update for {}", entity.get()->get_display_name());
			}
		}
		
		for (auto& entity : entities)
		{
			if (entity.get()->is_entity(EntityClass::Unit))
			{
				Unit* unit1 = static_cast<Unit*>(entity.get());
				if (CollisionElement* ce1 = unit1->get_element<CollisionElement>())
				{
					std::vector<Sector*> near_sectors = active_map.get()->get_sector_grid(unit1->get_map_location(), 1);
					for (auto& sector : near_sectors)
					{
						for (auto& entity2 : sector->entities)
						{
							if (!entity2.get()->is_entity(EntityClass::Unit))
							{
								continue;
							}
							Unit* unit2 = static_cast<Unit*>(entity2.get());
							if (CollisionElement* ce2 = unit2->get_element<CollisionElement>())
							{
								if (unit1 != unit2)
								{
									handle_collision(*ce1, *ce2, delta_time);
								}
							}
						}
					}
				}
			}
		}
		
		std::vector<RenderGroup> render_groups = {RenderGroup::PreGame, RenderGroup::Game, RenderGroup::PostGame, RenderGroup::UI, RenderGroup::Debug};
		for (const RenderGroup& rg : render_groups)
		{
			if (rg == RenderGroup::PostGame)
			{
				if (GFX::top_bar_image && GFX::bottom_bar_image)
				{
					ForgetteDirectX::draw_image(GFX::top_bar_image, {0, 0});
					ForgetteDirectX::draw_image(GFX::bottom_bar_image, {0, 1224});
				}
				
				continue;
			}
			
			for (auto& entity : entities)
			{
				if (entity.get()->should_render_update) 
				{ 
					entity.get()->render_update(rg);
				}
			}
		}
		
		for (auto& [priority, function_vector] : render_functions)
		{
			function_vector.erase(
            	std::remove_if(function_vector.begin(), function_vector.end(), [delta_time](const std::function<bool(float)>& render_function) 
            	{
                	return !render_function(delta_time); // Remove functions that return false
            	}), 
            	function_vector.end());
		}
		
		bool draw_cursor = true;
		if (draw_cursor && GFX::cursor_image)
		{
			ForgetteDirectX::draw_image(GFX::cursor_image, input::get_cursor_screen_location(), 0.5f);
		}
	}
	
	void Engine::menu_loop()
	{
		
	}
	
	std::wstring Engine::get_application_dir()
	{
		wchar_t path[MAX_PATH];
		GetModuleFileNameW(NULL, path, MAX_PATH);
		std::wstring path_wstring = std::wstring(path);
		size_t pos = path_wstring.find_last_of(L"\\/");
		return (std::wstring::npos == pos) ? L"" : path_wstring.substr(0, pos);
	}
}