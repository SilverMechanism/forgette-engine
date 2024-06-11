module;
#ifdef WIN64
	#include <d2d1_1.h>
	#include <cassert>
#endif
export module sprite;
#ifdef WIN64
	import directx;
#endif
import std;
import gfx;
import core;
import database;

export
{
	class Sprite
	{
	public:
		Sprite();
		
		Sprite(std::string sprite_name);
		Sprite(std::string sprite_name, coordinates<float> _draw_size);
		Sprite(std::string sprite_name, coordinates<int> _atlas_location, coordinates<int> _atlas_size);
		
		Sprite(std::wstring sprite_path);
		
		// Load the image from disk, returns false if failed
		bool load(std::string sprite_name);
		bool load(std::wstring sprite_path);
		
		bool use_atlas = false;
		coordinates<int> atlas_location;
		coordinates<int> atlas_size;
		coordinates<float> draw_size;
		
		// Returns a negative number if it could not be retrieved
		coordinates<float> get_image_dimensions();
		
		void render_to_screen(coordinates<float> screen_location);
		
		void render_to_map(coordinates<float> map_location, bool force_render);
		
		GFX::raw_image* get_image();
	private:
		ptr::keeper<GFX::raw_image> image;
		coordinates<float> dimensions;
	};
}

Sprite::Sprite()
{
	image = ptr::keeper<GFX::raw_image>(nullptr);
}

Sprite::Sprite(std::string sprite_name, coordinates<int> _atlas_location, coordinates<int> _atlas_size) :
	Sprite(sprite_name)
{
	atlas_location = _atlas_location;
	atlas_size = _atlas_size;
}

#ifdef WIN64
Sprite::Sprite(std::string sprite_name)
{
	assert(load(sprite_name));
	draw_size = get_image_dimensions();
}

Sprite::Sprite(std::string sprite_name, coordinates<float> _draw_size)
{
	draw_size = _draw_size;
	assert(load(sprite_name));
}

Sprite::Sprite(std::wstring sprite_path)
{
	assert(load(sprite_path));
}

bool Sprite::load(std::string sprite_name)
{
	std::string sprite_path = database::get_sprite_path(sprite_name);
	image = ptr::keeper<GFX::raw_image>(GFX::load_image_file(get_exe_dir() + string_to_wstring(sprite_path)));
	return image.get();
}

bool Sprite::load(std::wstring sprite_path)
{
	std::cout << "[SPRITE] WARNING: Loading a sprite directly from a path using load() is deprecated.\n[SPRITE] Path used: " << wstring_to_string(sprite_path) << std::endl << std::endl;
	image = ptr::keeper<GFX::raw_image>(GFX::load_image_file(sprite_path));
	return image.get();
}

coordinates<float> Sprite::get_image_dimensions()
{
	if (dimensions)
	{
		return dimensions;
	}
	
	if (atlas_size)
	{
		dimensions = {static_cast<float>(atlas_size.x), static_cast<float>(atlas_size.y)};
	}
	else if (image.get())
	{
		D2D1_SIZE_F d2d1_dimensions = image.get()->GetSize();
		dimensions = {d2d1_dimensions.width, d2d1_dimensions.height};
	}
	
	return dimensions;
}

// The sprite will be rendered from the bottom-left-most pixel up
void Sprite::render_to_screen(coordinates<float> screen_location)
{
/*	coordinates dimensions = get_dimensions();
	D2D1_RECT_F source_rect = D2D1::RectF(0.0f, 0.0f, dimensions.x, dimensions.y);
	D2D1_RECT_F screen_rect = D2D1::RectF(location.x - dimensions.x, location.y - dimensions.y, location.x, location.y);
	GFX::get_render_target()->DrawBitmap(bitmap.get(), screen_rect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, source_rect);*/
}

// map_location: the location of the sprite on the map
// game_map: a reference to the map itself
// force_render: if true, will render the sprite without sector checks
void Sprite::render_to_map(coordinates<float> map_location, bool force_render/*, const GameMap &game_map*/)
{
	if (!force_render/* && !game_map.sector_check(map_location, GFX::render_viewpoint)*/)
	{
		return;
	}
	
	if (image.get())
	{
		if (atlas_size)
		{
			ForgetteDirectX::draw_sprite_to_map
			(
				image.get(),
				draw_size,
				map_location,
				atlas_location,
				atlas_size
			);
		}
		else
		{
			ForgetteDirectX::draw_sprite_to_map
			(
				image.get(), 
				draw_size, 
				map_location
			);
		}
	}
	else
	{
		std::cout << "SPRITE: My image isn't valid!" << std::endl;
	}
}
#endif

GFX::raw_image* Sprite::get_image()
{
	return image.get();
}