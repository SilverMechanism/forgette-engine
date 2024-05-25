module;
export module game_map;
import core;
import std;
import entity;
import sprite;
import gfx;
import database;
import sector;
import entity;

export
{
	const coordinates<int> tile_size = coordinates<int>(256, 128);
	const coordinates<int> tile_atlas_size = coordinates<int>(256, 128);
	
	struct tile
	{
		ptr::watcher<Sprite> tile_sprite;
	};
	
	class GameMap
	{
	public:
		GameMap(coordinates<float> _sector_size, std::string _name, const coordinates<std::uint16_t> map_size_in_sectors);
		~GameMap();
		
		const coordinates<float> map_size = {0.0f, 0.0f};
		
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
		
		void generate_flatmap(std::string tile_name);
		
		void render_tiles();
		
	protected:
		bool was_setup = false;
		
	private:
		std::vector<std::vector<Sector>> sectors;
		std::string name{ "Just Another Forgette Map" };
	};
}

GameMap::GameMap(coordinates<float> _sector_size, std::string _name, const coordinates<std::uint16_t> map_size_in_sectors) : 
	sector_size(_sector_size),
	name(_name),
	map_size({static_cast<float>(map_size_in_sectors.x) * _sector_size.x, static_cast<float>(map_size_in_sectors.y) * _sector_size.y})
{
	Sector default_sector;
}

GameMap::~GameMap()
{
	
}

void GameMap::make_tile(std::string tile_name, std::wstring path, bool replace)
{
	if (!replace && tileset.find(tile_name) != tileset.end())
	{
		return;
	}
	
	ptr::keeper<Sprite> new_sprite(new Sprite(path));
	tileset[tile_name] = std::move(new_sprite);
}

std::string GameMap::get_name()
{
	return name;
}

coordinates<float> GameMap::get_extent()
{
	return {1.0f, 1.0f};
	if (sectors.size() == 0)
	{
		return {0.0f, 0.0f};
	}
	
	return {sector_size.x*static_cast<float>(sectors.size()), sector_size.y*static_cast<float>(sectors[0].size())};
}

coordinates<float> GameMap::get_sector_size()
{
	return sector_size;
}

bool GameMap::get_was_setup()
{
	return was_setup;
}

void GameMap::finish_setup()
{
	was_setup = true;
}

void GameMap::generate_flatmap(std::string tile_name)
{
	database::TileDefinition tile_def = database::tiles_get_entry(tile_name);
	tile_def.path = wstring_to_string(get_exe_dir()) + tile_def.path;
	std::cout << tile_def.path << std::endl;
	
	// *** TESTING HARDCODE ***
	
	
	auto tileset_iterator = tileset.find(tile_def.name);
	// ptr::keeper<Sprite>* sprite_keeper_ptr = nullptr;
	
	// We gotta make the sprite first
	if (tileset_iterator == tileset.end())
	{
		tileset[tile_def.name] = 
			ptr::keeper<Sprite>
			(
				new Sprite
				(
					string_to_wstring(tile_def.path),
					coordinates<int>(0,0) + (tile_atlas_size * tile_def.index),
					tile_size/tile_def.scale
				)
			);
			
		tileset[tile_def.name].get()->draw_size = tile_size;
		
		// sprite_keeper_ptr = &tileset[tile_def.name];
	}
	else
	{
		// sprite_keeper_ptr = &tileset_iterator->second;
	}
	
	// if (sprite_keeper_ptr->get())
	// {
		coordinates<float> map_size_iterator = coordinates<float>(-map_size.x, -map_size.y);
		while (map_size_iterator.x < map_size.x)
		{
			while (map_size_iterator.y < map_size.y)
			{
				// std::println("Made a tile at {}", std::string(map_size_iterator), std::string(map_size));
				
				tiles[map_size_iterator] = tile();
				tiles[map_size_iterator].tile_sprite = ptr::watcher<Sprite>(tileset[tile_def.name]);
				
				map_size_iterator.y += tile_size.y;
			}
			
			map_size_iterator.x += tile_size.x;
			map_size_iterator.y = -map_size.y;
		}
	// }
}

void GameMap::render_tiles()
{
	for (const auto &pair : tiles)
	{
		if (Sprite* tile_sprite = pair.second.tile_sprite.get())
		{
			tile_sprite->render_to_map(pair.first, true);
		}
	}
}