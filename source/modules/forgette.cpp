module;
#include <windows.h>
#include <assert.h>
export module forgette;
import gfx;
import std;
import windows;
import timers;
import entity;
import camera_entity;
import input;
import core;
import directx;
import lua_manager;
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
		
		void update_render_viewpoint(coordinates<float> new_viewpoint);
		
		Entity* player_viewpoint = nullptr;
		
		std::wstring get_application_dir();
		
		ptr::keeper<GameMap> active_map;
		
		template<typename T>
		T* spawn_entity(coordinates<float> world_location)
		{
			T* spawn = new T();
			ptr::keeper<Entity> new_keeper(static_cast<Entity*>(spawn));
			entities.push_back(std::move(new_keeper));

#ifdef _DEBUG
			std::println("Spawned {} at {}", spawn->get_display_name(), std::string(spawn->get_map_location()));
#endif

			spawn->set_map_location(world_location);
			return spawn;
		}
		
		template<typename T>
		T* spawn_entity(coordinates<float> world_location, ptr::watcher<T> &new_watcher)
		{
			T* spawn = new T();
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
			return spawn;
		}
		
		ptr::keeper<LuaManager> lua_manager = ptr::keeper<LuaManager>(nullptr);

	private:
		Mode active_mode = Mode::not_set;
		std::map<Mode, std::function<void()>> loop_functions;
		void map_loop();
		void menu_loop();
		ptr::watcher<CameraEntity> active_camera;
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
		
		// active_map.get()->generate_map_from_image(L"D:\\silmech\\blood_fields.bmp");
		
		lua_manager.get()->run_lua_script(script_path);
		
		spawn_entity<CameraEntity>(coordinates(0,0), active_camera);
		
		active_camera->lock_on_target = ptr::watcher<Unit>(entities[entities.size()-2]);
		std::println("{}", active_camera->lock_on_target->get_display_name());
		
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
		
		if (active_camera.get())
		{
			ForgetteDirectX::set_render_viewpoint(active_camera->get_map_location());
		}

		ForgetteDirectX::prerender();
		
		loop_functions[active_mode]();

		ForgetteDirectX::present(true);
	}
	
	void Engine::map_loop()
	{
		if (!active_map.get())
		{
			return;
		}
		
		active_map.get()->render_tiles();
		
		for (auto& entity : entities)
		{
			if (entity.get()->should_game_update) 
			{ 
				entity.get()->game_update(timer_manager->get_delta_time()); 
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