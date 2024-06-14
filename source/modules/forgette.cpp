module;
#include <windows.h>
#include <assert.h>
export module forgette;
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
import movement;
import unit;
import game_map;

export namespace Forgette
{
	enum class Mode : byte
	{
		not_set,
		in_menu,
		in_map
	};
	
	class Engine
	{
	public:
		Engine();
		~Engine();

		TimerManager* timer_manager;

		void initialize_graphics(HINSTANCE hInstance, int nCmdShow);
		
		// No params = load blank map
		void load_game_map(std::wstring script_path);

		void loop();

		bool healthy = true;

		std::vector<ptr::keeper<Entity>> entities;
		std::vector<ptr::keeper<Entity>> new_spawns;
		
		void update_render_viewpoint(coordinates<float> new_viewpoint);
		
		Entity* player_viewpoint = nullptr;
		
		std::wstring get_application_dir();
		
		ptr::keeper<GameMap> active_map;
		
		std::int64_t entity_counter = 0;
		
		std::map<std::uint8_t, std::vector<std::function<bool(float)>>> render_functions;
		
		template<typename T>
		void spawn_entity(coordinates<float> world_location)
		{
			T* spawn = new T();
			spawn->id = entity_counter;
			
			ptr::keeper<Entity> keeper = ptr::keeper<Entity>(spawn);
			
			if (Unit* unit = dynamic_cast<Unit*>(spawn))
			{
				unit->set_map_location(world_location);
			}
			
			new_spawns.push_back(std::move(keeper));
			
			spawn->on_spawn();
			entity_counter++;
		}
		
		template<typename T>
		void spawn_entity(coordinates<float> world_location, ptr::watcher<T> &new_watcher)
		{
			T* spawn = new T();
			spawn->id = entity_counter;
			
			ptr::keeper<Entity> keeper = ptr::keeper<Entity>(spawn);
			new_watcher = ptr::watcher<T>(keeper);
			
			if (Unit* unit = dynamic_cast<Unit*>(spawn))
			{
				unit->set_map_location(world_location);
			}
			
			new_spawns.push_back(std::move(keeper));
			
			spawn->on_spawn();
			entity_counter++;
		}
		
		template<typename T>
		void spawn_entity(ptr::watcher<T> &new_watcher)
		{
			T* spawn = new T();
			spawn->id = entity_counter;
			
			ptr::keeper<Entity> keeper = ptr::keeper<Entity>(spawn);
			new_watcher = ptr::watcher<T>(keeper);
			
			new_spawns.push_back(std::move(keeper));
			
			spawn->on_spawn();
			entity_counter++;
		}
		
		template<typename T>
		void spawn_entity()
		{
			T* spawn = new T();
			spawn->id = entity_counter;
			new_spawns.push_back(ptr::keeper<Entity>(spawn));
			
			const ptr::keeper<Entity> &my_keeper = new_spawns.back();
			
			spawn->on_spawn();
			entity_counter++;
		}
		
		ptr::keeper<LuaManager> lua_manager = ptr::keeper<LuaManager>(nullptr);
		
		Player* create_player();

	private:
		Mode active_mode = Mode::not_set;
		std::map<Mode, std::function<void()>> loop_functions;
		void map_loop();
		void setup_map();
		void menu_loop();
		ptr::watcher<Player> local_player;
	};
}

export
{
	Forgette::Engine* get_engine();
	void set_engine(Forgette::Engine* engine);
}

std::unique_ptr<Forgette::Engine> global_engine = nullptr;

Player* Forgette::Engine::create_player()
{
	spawn_entity<Player>(coordinates(0,0), local_player);
	return local_player.get();
}

Forgette::Engine* get_engine()
{
	return global_engine.get();
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
		input::map_key("move_up", 0x0057, true);
		input::map_key("move_right", 0x0044, true);
		input::map_key("move_down", 0x0053, true);
		input::map_key("move_left", 0x0041, true);
		
		std::cout << 
		R"(
═════════════════════════════════════════════════════════════════════
█████████████████████████████████████████████████████████████████████

███████╗ ██████╗ ██████╗  ██████╗ ███████╗████████╗████████╗███████╗
██╔════╝██╔═══██╗██╔══██╗██╔════╝ ██╔════╝╚══██╔══╝╚══██╔══╝██╔════╝
█████╗  ██║   ██║██████╔╝██║  ███╗█████╗     ██║      ██║   █████╗  
██╔══╝  ██║   ██║██╔══██╗██║   ██║██╔══╝     ██║      ██║   ██╔══╝  
██║     ╚██████╔╝██║  ██║╚██████╔╝███████╗   ██║      ██║   ███████╗
╚═╝      ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚══════╝   ╚═╝      ╚═╝   ╚══════╝                                                                                                                                                                                                                  
█████████████████████████████████████████████████████████████████████
═════════════════════════════════════════════════════════════════════

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
			coordinates<float>(256.0f, 128.0f), 
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
		timer_manager->ProcessCallbacks();

		for (const auto& pair : input::active_down_binds)
		{
			pair.second.key_down_bindings[0]();
		}
		
		/* if (local_player.get())
		{
			ForgetteDirectX::set_render_viewpoint(local_player->get_controlled_unit()->get_map_location());
		} */
		
		loop_functions[active_mode]();

		ForgetteDirectX::present(true);
	}
	
	bool z_compare(const ptr::keeper<Entity> &a, const ptr::keeper<Entity> &b)
	{
	    Unit* unit1 = dynamic_cast<Unit*>(a.get());
	    Unit* unit2 = dynamic_cast<Unit*>(b.get());
	
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
	
	bool check_collision(const Unit &unit1, const Unit &unit2)
	{
		coordinates<float> pos1 = unit1.get_map_location();
		coordinates<float> pos2 = unit2.get_map_location();
		
		float distance = coordinates<float>(pos1.x - pos2.x, pos1.y - pos2.y).magnitude();
		float collision_distance = unit1.radius + unit2.radius;
		
		return (distance < collision_distance);
	}
	
	void handle_collision(Unit& unit1, Unit& unit2, float delta_time)
	{
		if (!unit1.collision_enabled || !unit2.collision_enabled)
		{
			return;
		}
		
	    coordinates<float> pos1 = unit1.get_map_location();
	    coordinates<float> pos2 = unit2.get_map_location();
	
	    float dx = pos1.x - pos2.x;
	    float dy = pos1.y - pos2.y;
	    float distance = std::sqrt(dx * dx + dy * dy);
	    float collision_distance = unit1.radius + unit2.radius;
	
	    if (distance < collision_distance)
	    {
	    	if (!unit1.collides_with.count(unit2.collision_group) 
	    		|| !unit2.collides_with.count(unit1.collision_group))
	    	{
	    		return;
	    	}
	    	
			const std::vector<std::int64_t>& ents1 = unit1.ignored_entities;
			const std::vector<std::int64_t>& ents2 = unit2.ignored_entities;
			if (std::find(ents1.begin(), ents1.end(), unit2.id) != ents1.end()
				|| std::find(ents2.begin(), ents2.end(), unit1.id) != ents2.end())
			{
				return;
			}
			
			unit1.on_collision(&unit2);
			unit2.on_collision(&unit1);

	        // Calculate the amount to rewind
	        float overlap = collision_distance - distance;
	        
	        // Normalize the direction vector
	        float nx = dx / distance;
	        float ny = dy / distance;
	
	        // Calculate the total velocities along the collision normal
	        float v1_dot_n = unit1.movement.velocity.x * nx + unit1.movement.velocity.y * ny;
	        float v2_dot_n = unit2.movement.velocity.x * nx + unit2.movement.velocity.y * ny;
	
	        // Calculate how much to rewind each unit based on their velocities and delta_time
	        float rewind_factor1 = v1_dot_n * delta_time;
	        float rewind_factor2 = v2_dot_n * delta_time;
	
	        // Adjust the positions by the smallest possible amount to stop the collision
	        pos1.x -= rewind_factor1 * nx;
	        pos1.y -= rewind_factor1 * ny;
	        pos2.x -= rewind_factor2 * nx;
	        pos2.y -= rewind_factor2 * ny;
			
			if (unit1.movement.velocity)
			{
				unit1.set_map_location(pos1);
			}
	        if (unit2.movement.velocity)
	        {
	        	unit2.set_map_location(pos2);
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
			entities.insert(
				entities.end(),
				std::make_move_iterator(new_spawns.begin()),
				std::make_move_iterator(new_spawns.end()));

			new_spawns.clear();
		}

		for (auto it = entities.begin(); it != entities.end(); /* no increment here */) 
		{
	        if (it->get()->pending_deletion) 
	        {
	            it->~keeper();
	            it = entities.erase(it);
	        } 
	        else 
	        {
	            ++it;
	        }
	    }
	    
		const float delta_time = timer_manager->get_delta_time();
		
		if (!active_map.get())
		{
			return;
		}
		
		active_map.get()->render_tiles();
		
		std::sort(entities.begin(), entities.end(), z_compare);
		
		for (auto &entity : entities)
		{
			if (Unit* unit1 = dynamic_cast<Unit*>(entity.get()))
			{
				for (auto &entity2 : entities)
				{
					if (Unit* unit2 = dynamic_cast<Unit*>(entity2.get()))
					{
						if (unit1 != unit2)
						{
							handle_collision(*unit1, *unit2, delta_time);
						}
					}
				}
			}
		}
		
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
			
			if (entity.get()->should_render_update) 
			{ 
				entity.get()->render_update();
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