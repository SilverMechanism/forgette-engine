module;
export module game_map;

import core;
import std;
import game_mode;

export class Sprite;
export class Sector;

// Concept to check if T is derived from GameMode
template<typename T>
concept GameModeChild = std::is_base_of_v<GameMode, T>;

export
{
	struct SectorInfo
	{
		coordinates<float> size;
		coordinates<float> center;
		Sector* sector = nullptr;
	};
	
	// const coordinates<int> tile_size = coordinates<int>(128, 64);
	// const coordinates<int> tile_atlas_size = coordinates<int>(256, 128);
	// const int tile_scale = 1;
	
	struct tile
	{
		ptr::watcher<Sprite> tile_sprite;
	};
	
	class GameMap
	{
	public:
		GameMap(coordinates<float> _sector_size, std::string _name, const coordinates<std::uint16_t> map_size_in_sectors);
		~GameMap();
		
		// If an exact match to a sector location, return sector immediately
		// Otherwise it will look for the sector that contains this point
		// Do not store this pointer, as it may be made invalid
		Sector* get_sector(coordinates<float> map_location);
		SectorInfo get_sector_info(coordinates<float> map_location);
		std::vector<Sector*> get_sector_grid(coordinates<float> map_location, int layer_size);
		std::vector<SectorInfo> get_sector_info_grid(coordinates<float> map_location, int layer_size);
		
		ptr::watcher<GameMode> active_mode;
		
		const coordinates<float> map_size = {0.0f, 0.0f};
		
		std::unordered_map<coordinates<float>, Sector> sectors;
		std::unordered_map<coordinates<float>, tile> tiles;
		std::unordered_map<std::string, ptr::keeper<Sprite>> tileset;
		
		const coordinates<float> sector_size { 1024.0f, 1024.0f };
		std::string get_name();
		
		// Get the extent of the map in physical units (ie inches)
		coordinates<float> get_extent();
		
		coordinates<float> get_sector_size();
		// virtual void setup() = 0;
		bool get_was_setup();
		
		void make_tile(std::string tile_name, std::wstring path, bool replace);
		
		void finish_setup();
		
		void generate_flatmap(std::string tile_name, int tiles_x, int tiles_y);
		void generate_map_from_image(const std::wstring &filepath);
		
		void render_tiles();
		
		const coordinates<float> tile_size = {36, 36}; // square yard

		template<GameModeChild T>
		void setup_game()
		{
			T* new_game_mode = new T();
			active_game_mode = ptr::keeper<GameMode>(new_game_mode);
			new_game_mode->start();
		}
		
	protected:
		bool was_setup = false;
		
	private:
		std::string name{ "Just Another Forgette Map" };
		
		ptr::keeper<GameMode> active_game_mode;
	};
}