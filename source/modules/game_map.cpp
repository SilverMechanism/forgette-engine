module;
#include <cassert>
export module game_map;
import core;
import std;
import entity;
import sprite;
import gfx;
import database;
import sector;
import entity;
import directx;

std::map<GFX::RGB8, std::string> pixel_names = {
	{GFX::RGB8(0, 255, 0), "grassy_dirt"},
	{GFX::RGB8(0, 100, 0), "grass"},
	{GFX::RGB8(0, 0, 0), "road"}
};

export
{
	const coordinates<int> tile_size = coordinates<int>(256, 128);
	const coordinates<int> tile_atlas_size = coordinates<int>(256, 128);
	const int tile_scale = 1;
	
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
		void generate_map_from_image(const std::wstring &filepath);
		
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

static ptr::keeper<Sprite> default_sprite_keeper;

void GameMap::generate_flatmap(std::string tile_name)
{
	const coordinates<int> map_size = {128, 128};
	const coordinates<float> tile_size = {36, 36};
	coordinates<float> map_location = {0, 0};
	
	/* Sprite* default_sprite = 
		new Sprite
		(
			get_exe_dir() + L"sprites\\environment\\misc\\default_tile.png",
			{0, 0},
			{128, 64}
		);
	default_sprite->draw_size = default_sprite->get_dimensions(); */
	
	Sprite* default_sprite = 
		new Sprite
		(
			get_exe_dir() + L"sprites\\environment\\misc\\default_tile.png"
		);
	
	default_sprite_keeper = ptr::keeper<Sprite>(default_sprite);
	
	for (int y = 0; y < map_size.y; y++)
	{
		map_location.y = map_location.y + tile_size.y;
		
		for (int x = 0; x < map_size.x; x++)
		{
			map_location.x = map_location.x + tile_size.x;
			
			tiles[map_location] = tile();
			tiles[map_location].tile_sprite = ptr::watcher<Sprite>(default_sprite_keeper);
		}
		
		map_location.x = 0;
	}
}

void GameMap::generate_map_from_image(const std::wstring &filepath)
{	
	std::vector<std::vector<GFX::RGB8>> colors = GFX::get_image_colors(filepath);
	
	int left_x = (static_cast<int>(colors[0].size()) * -128);
	int top_y = (static_cast<int>(colors.size()) * 128);
	
	coordinates<int> map_location = coordinates<int>(left_x, top_y);
	std::cout << "Map location: " << std::string(map_location) << std::endl;
	
	for (int y = 0; y < colors.size(); y++)
	{
		for (int x = 0; x < colors[y].size(); x++)
		{
			std::string tile_name = pixel_names[colors[y][x]];
			
			database::TileDefinition tile_def = database::tiles_get_entry(tile_name);
			tile_def.path = wstring_to_string(get_exe_dir()) + tile_def.path;
			// std::cout << tile_def.path << std::endl;
			
			if (tile_def.scale == 0)
			{
				std::cout << "WARNING: TILE DEFINITION SCALE SET TO 0" << std::endl;
			}
			
			auto tileset_iterator = tileset.find(tile_def.name);
			if (tileset_iterator == tileset.end())
			{
				std::cout << "Making sprite for " << tile_def.name << std::endl;
				tileset[tile_def.name] = 
					ptr::keeper<Sprite>
					(
						new Sprite
						(
							string_to_wstring(tile_def.path),
							{tile_atlas_size.x * tile_def.atlas_position_x, tile_atlas_size.y * tile_def.atlas_position_y},
							tile_size/tile_def.scale
						)
					);
					
				tileset[tile_def.name].get()->draw_size = tile_size * tile_scale;
			}
			
			tiles[map_location] = tile();
			tiles[map_location].tile_sprite = ptr::watcher<Sprite>(tileset[tile_def.name]);
			// std::cout << "Made a tile at " << std::string(map_location) << std::endl;
			map_location.x = map_location.x + tile_size.x*tile_scale;
			// std::cout << "Pixel (" << x << ", " << y << ")\nCoord (" << map_location.x << ", " << map_location.y << ")\n";
		}
		// std::cout << "Reset X to " << left_x << std::endl;
		map_location.x = left_x;
		map_location.y = map_location.y - tile_size.y*tile_scale;
	}
}

void GameMap::render_tiles()
{
	for (const auto &pair : tiles)
	{
		if (Sprite* tile_sprite = pair.second.tile_sprite.get())
		{
			ForgetteDirectX::draw_map_tile
			(
				tile_sprite->get_image(), 
				tile_sprite->get_dimensions(),
				pair.first,
				36
			);
			
			// tile_sprite->render_to_map(pair.first, true);
		}
		else
		{
			assert(true);
		}
	}
}