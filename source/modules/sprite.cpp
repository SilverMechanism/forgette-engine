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

export
{
	class Sprite
	{
	public:
		Sprite();
		
		Sprite(std::wstring path_to_image);
		Sprite(std::wstring path_to_image, coordinates<int> _atlas_location, coordinates<int> _atlas_size);
		
		// Load the image from disk, returns false if failed
		bool load(std::wstring path_to_image);
		
		bool use_atlas = false;
		coordinates<int> atlas_location;
		coordinates<int> atlas_size;
		coordinates<float> draw_size;
		
		// Returns a negative number if it could not be retrieved
		coordinates<float> get_dimensions();
		
		void render_to_screen(coordinates<float> screen_location);
		
		void render_to_map(coordinates<float> map_location, bool force_render);
	private:
		ptr::keeper<GFX::raw_image> image;
	};
}

Sprite::Sprite()
{
	image = ptr::keeper<GFX::raw_image>(nullptr);
}

Sprite::Sprite(std::wstring path_to_image, coordinates<int> _atlas_location, coordinates<int> _atlas_size) :
	Sprite(path_to_image)
{
	atlas_location = _atlas_location;
	atlas_size = _atlas_size;
}

#ifdef WIN64
Sprite::Sprite(std::wstring path_to_image)
{
	assert(load(path_to_image));
}

bool Sprite::load(std::wstring path_to_image)
{
	image = ptr::keeper<GFX::raw_image>(GFX::load_image_file(path_to_image));
	return image.get();
}

coordinates<float> Sprite::get_dimensions()
{
	coordinates<float> dimensions;
	
	if (atlas_size)
	{
		coordinates<float> dimensions = {static_cast<float>(atlas_size.x), static_cast<float>(atlas_size.y)};
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
		if (atlas_size && atlas_location)
		{
			ForgetteDirectX::draw_sprite_to_map
			(
				image.get(),
				draw_size,
				map_location,
				ForgetteDirectX::default_projection,
				atlas_location,
				atlas_size
			);
		}
		else
		{
			ForgetteDirectX::draw_sprite_to_map
			(
				image.get(), 
				get_dimensions(), 
				map_location, 
				ForgetteDirectX::default_projection
			);
		}
	}
}
#endif
