module;
#ifdef WIN64
#include <d2d1_1.h>
#include <windows.h>
#include <wincodec.h>
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
	struct RGB8
	{
		RGB8(std::uint8_t r, std::uint8_t g, std::uint8_t b)
		{
			red = r;
			green = g;
			blue = b;
		}
		
		RGB8()
		{
			red = 0;
			green = 0;
			blue = 0;
		}
		
		std::uint8_t red;
		std::uint8_t green;
		std::uint8_t blue;
		
		bool operator<(const RGB8& other) const {
            return std::tie(red, green, blue) < std::tie(other.red, other.green, other.blue);
        }
	};
	
	const coordinates<int> floor_size = {256, 128};
	
	typedef ID2D1Bitmap raw_image;
	
	bool initialize();
	
	raw_image* load_image_file(const std::wstring &filepath);
	
	std::vector<std::vector<RGB8>> get_image_colors(const std::wstring &filepath);
	
	raw_image* cursor_image = nullptr;
	raw_image* top_bar_image = nullptr;
	raw_image* bottom_bar_image = nullptr;
}

#ifdef WIN64
bool GFX::initialize()
{
	// win_compat::Window &window = win_compat::Window::instance();
	
	ForgetteDirectX::initialize();
	
	cursor_image = load_image_file(get_exe_dir() + L"sprites\\silmech_cursor.png");
	top_bar_image = load_image_file(get_exe_dir() + L"sprites\\top_bar.png");
	bottom_bar_image = load_image_file(get_exe_dir() + L"sprites\\bottom_bar.png");
	
	return true;
}

GFX::raw_image* GFX::load_image_file(const std::wstring &filepath)
{
	return ForgetteDirectX::create_bitmap_from_file(filepath);
}

namespace GFX
{
	std::vector<std::vector<RGB8>> get_image_colors(const std::wstring &filepath)
	{
	    IWICBitmap* pBitmap = ForgetteDirectX::create_wicmap_from_file(filepath);
	    std::vector<std::vector<RGB8>> rgb_values;
	
	    IWICBitmapLock* pLock = nullptr;
	    WICRect rcLock = { 0, 0, 0, 0 };
	    pBitmap->GetSize((UINT*)&rcLock.Width, (UINT*)&rcLock.Height);
	
	    HRESULT hr = pBitmap->Lock(&rcLock, WICBitmapLockRead, &pLock);
	    if (SUCCEEDED(hr)) {
	        UINT width = rcLock.Width;
	        UINT height = rcLock.Height;
	        rgb_values.resize(height, std::vector<RGB8>(width));
	
	        UINT cbBufferSize = 0;
	        BYTE* pv = nullptr;
	        pLock->GetDataPointer(&cbBufferSize, &pv);
	
	        UINT stride = (width * 3 + 3) & ~3;
	        // pBitmap->GetStride(&stride);
	
	        for (UINT y = 0; y < height; ++y) {
	            for (UINT x = 0; x < width; ++x) {
	                BYTE* pixel = pv + y * stride + x * 3; // Calculate pixel location
	                RGB8 color = { pixel[2], pixel[1], pixel[0] }; // RGB order
	                rgb_values[y][x] = color; // Note: [y][x] to match typical image layout
	                // std::cout << "Pixel (" << x << ", " << y << "): " << (int)color.red << " " << (int)color.green << " " << (int)color.blue << std::endl;
	            }
	        }
	        pLock->Release();
	    }
	
	    // Release the bitmap
	    pBitmap->Release();
	
	    return rgb_values;
	}
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
