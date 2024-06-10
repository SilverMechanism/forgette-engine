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

// Fade function as defined by Ken Perlin. This eases coordinate values
// so that they will "ease" towards integral values. This ends up smoothing
// the final output.
float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// Linear interpolation function.
float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

// Gradient function. Converts lower 4 bits of hash code into 12 gradient directions.
float grad(int hash, float x, float y) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// Permutation vector. The same as the one in Ken Perlin's reference implementation.
int permutation[] = {151,160,137,91,90,15,131,13,201,95,
    96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,
    23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,
    68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,
    229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,
    143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,
    169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,
    3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,
    85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,
    170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,
    43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,
    185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,
    191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,
    31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,
    254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,
    66,215,61,156,180};

std::vector<int> p(512);

void init_perlin() {
    for (int i = 0; i < 256; i++) {
        p[256 + i] = p[i] = permutation[i];
    }
}

float perlin(float x, float y) {
    int X = (int)std::floor(x) & 255;
    int Y = (int)std::floor(y) & 255;
    x -= std::floor(x);
    y -= std::floor(y);
    float u = fade(x);
    float v = fade(y);
    int A = p[X] + Y;
    int AA = p[A];
    int AB = p[A + 1];
    int B = p[X + 1] + Y;
    int BA = p[B];
    int BB = p[B + 1];

    return lerp(v, lerp(u, grad(p[AA], x, y), grad(p[BA], x - 1, y)),
                lerp(u, grad(p[AB], x, y - 1), grad(p[BB], x - 1, y - 1)));
}

std::map<GFX::RGB8, std::string> pixel_names = {
	{GFX::RGB8(0, 255, 0), "grassy_dirt"},
	{GFX::RGB8(0, 100, 0), "grass"},
	{GFX::RGB8(0, 0, 0), "road"}
};

export
{
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
		
		void generate_flatmap(std::string tile_name, int tiles_x, int tiles_y);
		void generate_map_from_image(const std::wstring &filepath);
		
		void render_tiles();
		
		const coordinates<float> tile_size = {36, 36}; // square yard
		
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
}

void GameMap::generate_map_from_image(const std::wstring &filepath)
{	
	/* std::vector<std::vector<GFX::RGB8>> colors = GFX::get_image_colors(filepath);
	
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
	} */
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