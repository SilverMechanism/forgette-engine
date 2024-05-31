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
import game_map;
import debug_unit;
import villager;
import movement;

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
		
		void update_render_viewpoint(coordinates<float> new_viewpoint);
		
		Entity* player_viewpoint = nullptr;
		
		std::wstring get_application_dir();
		
		ptr::keeper<GameMap> active_map;
		
		std::int64_t entity_counter = 0;
		
		template<typename T>
		ptr::watcher<Entity> spawn_entity(coordinates<float> world_location)
		{
			T* spawn = new T();
			spawn->id = entity_counter;
			ptr::keeper<Entity> new_keeper(static_cast<Entity*>(spawn));
			entities.push_back(std::move(new_keeper));

#ifdef _DEBUG
			std::println("Spawned {} at {}", spawn->get_display_name(), std::string(spawn->get_map_location()));
#endif

			spawn->set_map_location(world_location);
			spawn->on_spawn();
			entity_counter++;
			return ptr::watcher<Entity>(entities.back());
		}
		
		template<typename T>
		ptr::watcher<Entity> spawn_entity(coordinates<float> world_location, ptr::watcher<T> &new_watcher)
		{
			T* spawn = new T();
			spawn->id = entity_counter;
			entities.push_back(ptr::keeper<Entity>(spawn));
			
			const ptr::keeper<Entity> &my_keeper = entities.back();
			new_watcher = ptr::watcher<T>(my_keeper);
			
			#ifdef _DEBUG
				if (Entity* _spawn = new_watcher.get())
				{
					std::println("Spawned {} at {}", _spawn->get_display_name(), std::string(_spawn->get_map_location()));
				}
			#endif
			
			spawn->set_map_location(world_location);
			spawn->on_spawn();
			entity_counter++;
			return ptr::watcher<Entity>(entities.back());
		}
		
		ptr::keeper<LuaManager> lua_manager = ptr::keeper<LuaManager>(nullptr);

	private:
		Mode active_mode = Mode::not_set;
		std::map<Mode, std::function<void()>> loop_functions;
		void map_loop();
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
		
		loop_functions[Mode::in_map] = std::bind(&Engine::map_loop, this);
		
		LuaManager* new_lua_manager = new LuaManager;
		lua_manager = ptr::keeper<LuaManager>(new_lua_manager);
		
		input::map_key("primary", 0x0001, true);
		input::map_key("secondary", 0x0002, true);
		input::map_key("move_up", 0x0057, true);
		input::map_key("move_right", 0x0044, true);
		input::map_key("move_down", 0x0053, true);
		input::map_key("move_left", 0x0041, true);
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
		
		spawn_entity<Player>(coordinates(0,0), local_player);
		
		local_player->possess_unit(ptr::watcher<Entity>(spawn_entity<DebugUnit>(coordinates(0, 0))));
		
		spawn_entity<Villager>(coordinates(-100, -75));
		
		lua_manager.get()->run_lua_script(script_path);
		
		return;
	}

	void Engine::loop()
	{
		timer_manager->ParseTimers();
		timer_manager->ProcessCallbacks();

		for (const auto& pair : input::active_down_binds)
		{
			pair.second.key_down_bindings[0]();
		}
		
		if (local_player.get())
		{
			ForgetteDirectX::set_render_viewpoint(local_player->get_map_location());
		}

		ForgetteDirectX::prerender();
		
		loop_functions[active_mode]();

		ForgetteDirectX::present(true);
		
		if (local_player.get())
		{
			// std::cout << std::string(local_player->get_map_location()) << std::endl;
		}
	}
	
	bool z_compare(const ptr::keeper<Entity> &a, const ptr::keeper<Entity> &b)
	{
		if (!(a.get() && b.get()))
		{
			return false;
		}
		
		auto map_location_a = a.get()->get_map_location().isometric();
		auto map_location_b = b.get()->get_map_location().isometric();
			
		if (map_location_a.y == map_location_b.y) 
		{
		    return map_location_a.x < map_location_b.x;
	    }
	    
	    return map_location_a.y < map_location_b.y;
	    
	    return false;
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
	    coordinates<float> pos1 = unit1.get_map_location();
	    coordinates<float> pos2 = unit2.get_map_location();
	
	    float dx = pos1.x - pos2.x;
	    float dy = pos1.y - pos2.y;
	    float distance = std::sqrt(dx * dx + dy * dy);
	    float collision_distance = unit1.radius + unit2.radius;
	
	    if (distance < collision_distance)
	    {
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
	
	void Engine::map_loop()
	{
		float delta_time = timer_manager->get_delta_time();
		
		// coordinates<float> mouse_pos = input::get_cursor_map_location();
		// std::cout << "Mouse position: (" << mouse_pos.x << ", " << mouse_pos.y << ")\n";
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