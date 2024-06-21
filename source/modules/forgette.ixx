module;
#include <windows.h>
#include <assert.h>
export module forgette;

import core;

export class LuaManager;
export class Entity;
export class Unit;
export class TimerManager;
export class GameMap;
export class Player;

export
{
	namespace Forgette
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
			
			void add_unit_to_sector(coordinates<float> map_location, const ptr::keeper<Entity>& keeper);
			void add_unit_to_sector(coordinates<float> map_location, std::int64_t entity_id);
			
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
			
			LuaManager* get_lua_manager();
			
			Player* create_player();
			
			GameMap* get_map();
	
		private:
			Mode active_mode = Mode::not_set;
			std::map<Mode, std::function<void()>> loop_functions;
			void map_loop();
			void setup_map();
			void menu_loop();
			ptr::watcher<Player> local_player;
			ptr::keeper<LuaManager> lua_manager;
		
		// Spawn templates //
		public:
			template<typename T>
			void spawn_entity(coordinates<float> world_location)
			{
				T* spawn = new T();
				spawn->id = entity_counter;
				
				ptr::keeper<Entity> keeper = ptr::keeper<Entity>(spawn);
				
				if constexpr (std::is_base_of_v<Unit, T>)
				{
					Unit* unit = static_cast<Unit*>(spawn);
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
				
				if constexpr (std::is_base_of_v<Unit, T>)
				{
					Unit* unit = static_cast<Unit*>(spawn);
					unit->set_map_location(world_location);
				}
				
				new_spawns.push_back(std::move(keeper));
				
				spawn->on_spawn();
				entity_counter++;
			}
			
			template<typename T>
			void spawn_entity(ptr::watcher<T> &new_watcher)
			{
				static_assert(!std::is_base_of_v<Unit, T>, "T must not be derived from Unit");
				
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
				static_assert(!std::is_base_of_v<Unit, T>, "T must not be derived from Unit");
				
				T* spawn = new T();
				spawn->id = entity_counter;
				new_spawns.push_back(ptr::keeper<Entity>(spawn));
				
				const ptr::keeper<Entity> &my_keeper = new_spawns.back();
				
				spawn->on_spawn();
				entity_counter++;
			}
			
			template<typename T>
			void get_entity(std::int64_t id, ptr::watcher<T>& new_watcher, bool new_spawn=false)
			{
				if (!new_spawn)
				{
					for (const auto& entity : entities)
					{
						if (entity.get()->id == id)
						{
							new_watcher = ptr::watcher<T>(entity);
							return;
						}
					}
				}
				else
				{
					for (const auto& entity : new_spawns)
					{
						if (entity.get()->id == id)
						{
							new_watcher = ptr::watcher<T>(entity);
							return;
						}
					}
				}
			}
			
		};
	}
	
	// No namespace
	Forgette::Engine* get_engine();
	void set_engine(Forgette::Engine* engine);
}