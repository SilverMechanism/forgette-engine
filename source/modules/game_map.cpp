module;
#include <cassert>
module game_map;

import core;
import std;
import entity;
import sprite;
import gfx;
import database;
import sector;
import entity;
import directx;
import game_state;
import game_mode;

import :perlin;

std::map<GFX::RGB8, std::string> pixel_names = {
	{GFX::RGB8(0, 255, 0), "grassy_dirt"},
	{GFX::RGB8(0, 100, 0), "grass"},
	{GFX::RGB8(0, 0, 0), "road"}
};

GameMap::GameMap(coordinates<float> _sector_size, std::string _name, const coordinates<std::uint16_t> map_size_in_sectors) :
	sector_size(_sector_size),
	name(_name),
	map_size({ static_cast<float>(map_size_in_sectors.x) * _sector_size.x, static_cast<float>(map_size_in_sectors.y) * _sector_size.y })
{
	int half_map_size_x = map_size_in_sectors.x / 2;
	int half_map_size_y = map_size_in_sectors.y / 2;

	for (int i = -half_map_size_x; i < half_map_size_x; ++i)
	{
		for (int j = -half_map_size_y; j < half_map_size_y; ++j)
		{
			coordinates<float> sector_center = { ((i + 0.5f) * _sector_size.x), ((j + 0.5f) * _sector_size.y) };
			sectors[sector_center] = Sector();
		}
	}
}

Sector* GameMap::get_sector(coordinates<float> map_location)
{
	// Is it a match?
	auto iterator = sectors.find(map_location);
	if (iterator != sectors.end())
	{
		return &iterator->second;
	}

	// Find nearest sector using the correct sector index calculation
	int sector_index_x = static_cast<int>(std::floor(map_location.x / sector_size.x));
	int sector_index_y = static_cast<int>(std::floor(map_location.y / sector_size.y));

	coordinates<float> sector_location = { ((sector_index_x + 0.5f) * sector_size.x), ((sector_index_y + 0.5f) * sector_size.y) };

	iterator = sectors.find(sector_location);
	if (iterator != sectors.end())
	{
		return &iterator->second;
	}
	else
	{
		return nullptr;
	}
}

SectorInfo GameMap::get_sector_info(coordinates<float> map_location)
{
	SectorInfo sector_info;
	sector_info.size = sector_size;
	
	// Is it a match?
	auto iterator = sectors.find(map_location);
	if (iterator != sectors.end())
	{
		sector_info.center = map_location;
		sector_info.sector = &iterator->second;
		return sector_info;
	}

	// Find nearest sector using the correct sector index calculation
	int sector_index_x = static_cast<int>(std::floor(map_location.x / sector_size.x));
	int sector_index_y = static_cast<int>(std::floor(map_location.y / sector_size.y));

	coordinates<float> sector_location = { ((sector_index_x + 0.5f) * sector_size.x), ((sector_index_y + 0.5f) * sector_size.y) };

	iterator = sectors.find(sector_location);
	if (iterator != sectors.end())
	{
		sector_info.center = sector_location;
		sector_info.sector = &iterator->second;
	}
	
	return sector_info;
}

std::vector<Sector*> GameMap::get_sector_grid(coordinates<float> map_location, int layer_size)
{
	std::vector<Sector*> sector_grid;

	int sector_index_x = static_cast<int>(std::floor(map_location.x / sector_size.x));
	int sector_index_y = static_cast<int>(std::floor(map_location.y / sector_size.y));

	// Iterate over the grid based on the layer size
	for (int dx = -layer_size; dx <= layer_size; ++dx)
	{
		for (int dy = -layer_size; dy <= layer_size; ++dy)
		{
			coordinates<float> sector_location = { ((sector_index_x + dx + 0.5f) * sector_size.x), ((sector_index_y + dy + 0.5f) * sector_size.y) };

			Sector* sector = get_sector(sector_location);
			if (sector != nullptr)
			{
				sector_grid.push_back(sector);
			}
		}
	}

	return sector_grid;
}

std::vector<SectorInfo> GameMap::get_sector_info_grid(coordinates<float> map_location, int layer_size)
{
	std::vector<SectorInfo> sector_grid;

	int sector_index_x = static_cast<int>(std::floor(map_location.x / sector_size.x));
	int sector_index_y = static_cast<int>(std::floor(map_location.y / sector_size.y));

	// Iterate over the grid based on the layer size
	for (int dx = -layer_size; dx <= layer_size; ++dx)
	{
		for (int dy = -layer_size; dy <= layer_size; ++dy)
		{
			coordinates<float> sector_location = { ((sector_index_x + dx + 0.5f) * sector_size.x), ((sector_index_y + dy + 0.5f) * sector_size.y) };

			Sector* sector = get_sector(sector_location);
			if (sector != nullptr)
			{
				SectorInfo sector_info;
				sector_info.center = sector_location;
				sector_info.size = sector_size;
				sector_info.sector = sector;
				sector_grid.push_back(sector_info);
			}
		}
	}

	return sector_grid;
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
static ptr::keeper<Sprite> default_sprite2_keeper;
static ptr::keeper<Sprite> default_sprite3_keeper;

void GameMap::generate_flatmap(std::string tile_name, int tiles_x, int tiles_y)
{
	float frequency = 0.1f;
	
    if (!(tiles_y % 2) || !(tiles_x % 2))
    {
        assert("Number of tiles on map must be an even number");
    }
    
    tiles_y /= 2;
    tiles_x /= 2;
    
    coordinates<float> map_location = {-tiles_x * tile_size.x, -tiles_y * tile_size.y};
    
    Sprite* default_sprite = 
        new Sprite(
            get_exe_dir() + L"sprites\\environment\\misc\\default_tile.png"
        );
        
    Sprite* default_sprite2 = 
        new Sprite(
            get_exe_dir() + L"sprites\\environment\\misc\\default_tile3.png"
        );
        
    Sprite* default_sprite3 = 
    new Sprite(
        get_exe_dir() + L"sprites\\environment\\misc\\default_tile4.png"
    );
    
    init_perlin(); // Initialize Perlin noise
    
    default_sprite_keeper = ptr::keeper<Sprite>(default_sprite);
    default_sprite2_keeper = ptr::keeper<Sprite>(default_sprite2);
    default_sprite3_keeper = ptr::keeper<Sprite>(default_sprite3);
    
    for (int y = -tiles_y; y < tiles_y; y++)
    {
        map_location.y = map_location.y + tile_size.y;
        
        for (int x = -tiles_x; x < tiles_x; x++)
        {
            map_location.x = map_location.x + tile_size.x;
            
            tiles[map_location] = tile();
            
            // Using Perlin noise to determine tile type, with adjustable frequency
            float noise_value = perlin(x * frequency, y * frequency);
            if (noise_value < 0.1)
            {
                tiles[map_location].tile_sprite = ptr::watcher<Sprite>(default_sprite_keeper);
            }
            else
            {
            	if (noise_value < 0.3)
            	{
            		tiles[map_location].tile_sprite = ptr::watcher<Sprite>(default_sprite2_keeper);
            	}
                else
                {
                	tiles[map_location].tile_sprite = ptr::watcher<Sprite>(default_sprite3_keeper);
                }
            }
        }
        
        map_location.x = -tiles_x * tile_size.x;
    }
    
    // setup_game<GameMode>();
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
				tile_sprite->get_image_dimensions(),
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