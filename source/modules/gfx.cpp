module;
#ifdef WIN64
#include <d2d1_1.h>
#endif
export module gfx;
#ifdef WIN64
	import windows;
	import directx;
#elif defined(__APPLE__)
	import metal;
#endif
import std;
import core;

export namespace GFX
{
	const coordinates<int> floor_size = {256, 128};
	
	typedef ID2D1Bitmap raw_image;
	
	bool initialize();
	
	raw_image* load_image_file(const std::wstring &filepath);
}

#ifdef WIN64
bool GFX::initialize()
{
	// win_compat::Window &window = win_compat::Window::instance();
	
	ForgetteDirectX::initialize();
	
	return true;
}

GFX::raw_image* GFX::load_image_file(const std::wstring &filepath)
{
	return ForgetteDirectX::create_bitmap_from_file(filepath);
}
#endif

// Platform change =================================================== //

#ifdef __APPLE__
bool GFX::initialize()
{
	return false;
}

raw_image* GFX::load_image_file(std::wstring filepath)
{
	return nullptr;
}
#endif
