module;
#include <dwrite_3.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <windows.h>
#include <wincodec.h>
#include <cassert>
#include <d2d1effects_1.h>
#include <d2d1_1helper.h>
export module directx;
import windows;
import std;
import core;

using Microsoft::WRL::ComPtr;

ComPtr<IDXGIFactory2> dxgi_factory;
ComPtr<IDXGIDevice> dxgi_device;
ComPtr<IDXGIAdapter> dxgi_adapter;
ComPtr<IDXGISurface> dxgi_surface;
ComPtr<IDXGISwapChain1> swap_chain;

ComPtr<ID2D1Factory1> d2d1_factory;
ComPtr<ID2D1Device> d2d1_device;
ComPtr<ID2D1DeviceContext> d2d1_device_context;
std::unique_ptr<ID2D1Bitmap> logo;

ComPtr<IWICBitmapDecoder> decoder;
ComPtr<IWICImagingFactory> wic_factory;
ComPtr<IWICFormatConverter> tga_converter;

ComPtr<IDWriteFactory5> dwrite_factory;
ComPtr<IDWriteFontFile> old_english_font_file;
ComPtr<IDWriteFontFace> old_english_font_face;
ComPtr<ID2D1Effect> specular;

enum DrawStage
{
	none,
	game,
	ui,
	present
};

DrawStage draw_stage = DrawStage::none;

class WO_DestroyObserver : public win_compat::WindowObserver
{
	virtual bool on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override
	{
		// PostQuitMessage(0);

		return true;
	}
};

std::shared_ptr<WO_DestroyObserver> DestroyObserver;

// Print message plus the HR error info after
void check_hr(std::string message, HRESULT hr)
{
	if (FAILED(hr))
	{
		std::system_error error(hr, std::system_category(), message);
		std::string message = error.what();
		std::println("\nFatal Error: {}", message);
		assert(true);
	}
}

export namespace ForgetteDirectX
{
	ComPtr<ID2D1SolidColorBrush> sc_brush;
	ComPtr<ID2D1SolidColorBrush> black_brush;
	ComPtr<ID2D1SolidColorBrush> generic_brush; // Be sure to set the color of this brush every time you use it.
	
	void draw_text(std::string text, coordinates<float> screen_position, float text_size, bool drop_shadow = false, bool zoom_independent = true, ColorParams color = {1.0f, 1.0f, 1.0f, 1.0f});
	void draw_image(ID2D1Bitmap* bitmap, coordinates<float> screen_location, float scale = 1.0f);
	
	coordinates<float> world_to_screen(coordinates<float> world_coords, float z = 0.0f, bool print_debug=false);
	coordinates<float> screen_to_world(coordinates<float> screen_coords);
	
	enum class ProjectionMode : byte
	{
		Ratio2_1,
		Ratio1_1
	};
	
	ProjectionMode default_projection = ProjectionMode::Ratio2_1;
	
	const D2D1_BITMAP_INTERPOLATION_MODE default_interp_mode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
	
	float get_zoom_level();
	
	void set_zoom_level(float new_zoom_level);
	
	std::unique_ptr<ID2D1RenderTarget> d2d1_render_target;
	std::unique_ptr<ID2D1BitmapRenderTarget> game_render_target;
	std::unique_ptr<ID2D1BitmapRenderTarget> ui_render_target;
	
	ID2D1RenderTarget* get_render_target()
	{
		return d2d1_render_target.get();
	}
	
	ID2D1RenderTarget* get_game_render_target()
	{
		return d2d1_render_target.get();
	}
	
	ID2D1RenderTarget* get_ui_render_target()
	{
		return d2d1_render_target.get();
	}
	
	void SetKeyDesc(std::string NewDesc);

	void prerender();
	
	// Returns true on success, false on failure
	bool initialize();

	void present(bool vsync);
	
	ID2D1Bitmap* create_bitmap_from_file(std::wstring filepath);
	IWICBitmap* create_wicmap_from_file(const std::wstring &filepath);

	// Returns the resolution as an {width, height} pair.
	coordinates<float> get_resolution();
	
	coordinates<float> get_render_viewpoint();
	
	void set_render_viewpoint(coordinates<float> new_viewpoint);
	
	void draw_sprite_to_map(ID2D1Bitmap* bitmap, coordinates<float> dimensions, coordinates<float> map_location, float z = 0.0f);
	void draw_sprite_to_map(ID2D1Bitmap* bitmap, coordinates<float> dimensions, coordinates<float> map_location, coordinates<float> atlas_location, coordinates<float> atlas_size, float z = 0.0f);
	
	void draw_map_tile(ID2D1Bitmap* bitmap, coordinates<float> dimensions, coordinates<float> map_location, float tile_size);
	
	void draw_square(ColorParams color, coordinates<float> top_left, float size, bool fill, bool zoom_independent=false);
	void draw_circle(ColorParams color, coordinates<float> location, float radius, bool zoom_independent=false);
	void draw_unit_shadow(coordinates<float> screen_location, float radius);
	
	void draw_facing_helper(const coordinates<float> direction = {1.0f, 0.0f});
}

namespace ForgetteDirectX
{
	const coordinates<float> tile_pixels = {64, 32};
	const coordinates<float> tile_size = {36, 36};
	
	coordinates<float> render_viewpoint;
	coordinates<float> viewpoint_anchor;
	float zoom_level {1.5f};
	
	void draw_image(ID2D1Bitmap* bitmap, coordinates<float> screen_location, float scale)
	{
		D2D1_SIZE_F bitmap_size = bitmap->GetSize();
		
		D2D1_RECT_F draw_rect = 
			D2D1::RectF(screen_location.x, screen_location.y, 
			screen_location.x+(bitmap_size.width*scale), screen_location.y+(bitmap_size.height*scale));
			
		d2d1_render_target->DrawBitmap(
	            bitmap,
	            draw_rect, 
	            1.0f,
	            default_interp_mode, 
	            NULL);
	}
	
	void draw_text(std::string text, coordinates<float> screen_position, float text_size, bool drop_shadow, bool zoom_independent, ColorParams color)
	{
	    coordinates<float> resolution = get_resolution();
	    HRESULT hr;
	
	    std::wstring wtext = string_to_wstring(text);
	    ComPtr<IDWriteTextFormat> text_format;
	    ComPtr<IDWriteTextLayout> text_layout;
	    
	    if (!zoom_independent)
	    {
	    	text_size *= zoom_level;
	    }
	
	    hr = dwrite_factory->CreateTextFormat(
	        L"Old English Text MT",
	        nullptr,
	        DWRITE_FONT_WEIGHT_NORMAL,
	        DWRITE_FONT_STYLE_NORMAL,
	        DWRITE_FONT_STRETCH_NORMAL,
	        text_size,
	        L"en-US",
	        &text_format);
	    check_hr("CreateTextFormat", hr);
	
	    hr = dwrite_factory->CreateTextLayout(
	        wtext.c_str(),
	        static_cast<UINT32>(wtext.length()),
	        text_format.Get(),
	        resolution.x,
	        resolution.y,
	        &text_layout);
	    check_hr("CreateTextLayout", hr);
	
	    assert(text_format);
	    assert(text_layout);
	    assert(sc_brush);
	    assert(d2d1_render_target);
	
	    // Get the text metrics
	    DWRITE_TEXT_METRICS text_metrics;
	    hr = text_layout->GetMetrics(&text_metrics);
	    check_hr("GetMetrics", hr);
	
	    // Calculate the centered position
	    float centered_x = screen_position.x - (text_metrics.width / 2.0f);
	    float centered_y = screen_position.y - (text_metrics.height / 2.0f);
	    
	    if (drop_shadow)
	    {
		    d2d1_render_target->DrawTextLayout(
		        D2D1::Point2F(centered_x+(text_size*0.025f), centered_y+(text_size*0.025f)),
		        text_layout.Get(),
		        black_brush.Get(),
		        D2D1_DRAW_TEXT_OPTIONS_NONE);
	    }
		
		generic_brush->SetColor({color.red, color.green, color.blue, color.alpha});
	    // Draw the text at the centered position
	    d2d1_render_target->DrawTextLayout(
	        D2D1::Point2F(centered_x, centered_y),
	        text_layout.Get(),
	        generic_brush.Get(),
	        D2D1_DRAW_TEXT_OPTIONS_NONE);
	}
	
	float get_zoom_level()
	{
		return zoom_level;
	}
	
	void set_zoom_level(float new_zoom_level)
	{
		if (new_zoom_level > 1.5f || new_zoom_level < 0.75f)
		{
			return;
		}
		
		zoom_level = new_zoom_level;
		if (false)
		{
			std::cout << "New zoom level: " << zoom_level << std::endl;
		}
	}
	
	void set_render_viewpoint(coordinates<float> new_viewpoint)
	{
		render_viewpoint = new_viewpoint;
	}
	
	// Get the current render viewpoint in coordinates
	// This will usually be the player camera's location, but not always.
	coordinates<float> get_render_viewpoint()
	{
		return render_viewpoint;
	}
	
	// The position of the camera on the actual window dimensions
	// Usually, this will be the center of the screen, but may not be in special circumstances
	coordinates<float> get_viewpoint_anchor()
	{
		return viewpoint_anchor;
	}
	
	// Apply the zoom level to screen coordinates
	coordinates<float> apply_zoom(coordinates<float> screen_coords, const coordinates<float> screen_center)
	{
		screen_coords.x = (screen_coords.x - screen_center.x) * zoom_level + screen_center.x;
	    screen_coords.y = (screen_coords.y - screen_center.y) * zoom_level + screen_center.y;
	    return screen_coords;
	}
	
	coordinates<float> world_to_screen(coordinates<float> world_coords, float z, bool print_debug)
	{
		if (print_debug)
		{
			std::cout << "WORLD TO SCREEN\n=====" << std::endl;
			std::cout << "Pre transform: " << std::string(world_coords) << std::endl;
		}
		
	    coordinates<float> resolution = get_resolution();
	    coordinates<float> screen_center = { resolution.x / 2.0f, resolution.y / 2.0f };
	    coordinates<float> screen_coords;
	
	    // Calculate the relative position to the render viewpoint
	    world_coords.x = (world_coords.x - render_viewpoint.x);
	    world_coords.y = (world_coords.y - render_viewpoint.y);
	    
	    if (print_debug)
		{
			std::cout << "Relative to render viewpoint: " << std::string(world_coords) << std::endl;
		}
	
	    // Apply isometric transformation and Z-shift
	    if (!core_math::nearly_zero(z))
	    {
	    	world_coords = world_coords.isometric().z_shift(z * 5.0f);
	    	if (print_debug)
			{
				std::cout << "Isometric (plus zshift): " << std::string(world_coords) << std::endl;
			}
	    }
	    else
	    {
	    	world_coords = world_coords.isometric();
	    	if (print_debug)
			{
				std::cout << "Isometric (no zshift): " << std::string(world_coords) << std::endl;
			}
	    }
	
	    // Apply zoom level
	    world_coords.x *= zoom_level;
	    world_coords.y *= zoom_level;
	    
	    if (print_debug)
		{
			std::cout << "Post zoom: " << std::string(world_coords) << std::endl;
		}
	
	    // Adjust coordinates to be centered on the screen
	    screen_coords.x = world_coords.x + screen_center.x;
	    screen_coords.y = world_coords.y + screen_center.y;
	    
	    if (print_debug)
		{
			std::cout << "Final screen coords (added screen center): " << std::string(screen_coords) << std::endl;
			std::cout << "=====\n";
		}
	
	    return screen_coords;
	}
	
	coordinates<float> screen_to_world(coordinates<float> screen_coords)
	{
	    coordinates<float> resolution = get_resolution();
	    coordinates<float> world_coords;
	    
	    world_coords.x = screen_coords.x - (resolution.x/2);
	    world_coords.y = screen_coords.y - (resolution.y/2);
	    
	    world_coords = world_coords.world();
	    
	    return world_coords + render_viewpoint;
	}
	
	ID2D1GradientStopCollection* pGradientStops = nullptr;
	ID2D1RadialGradientBrush* pRadialGradientBrush = nullptr;
	D2D1_MATRIX_3X2_F shadow_rotation;
	
	void draw_square(ColorParams color, coordinates<float> top_left, float size, bool fill, bool zoom_independent)
	{
		coordinates<float> screen_top_left = world_to_screen(top_left, 0.0f);
		
		if (!zoom_independent)
		{
			size = size * zoom_level;
		}
		
		D2D1_RECT_F rect = D2D1::RectF(
			screen_top_left.x, screen_top_left.y,
			screen_top_left.x + size, screen_top_left.y + size
	    );
	    
	    // Create the rotation matrix
		auto rotation_matrix = D2D1::Matrix3x2F::Rotation(-45.0f, {screen_top_left.x, screen_top_left.y});
		
		// Create the scaling matrix
		auto scaling_matrix = D2D1::Matrix3x2F::Scale(1.0f, 0.5f, {screen_top_left.x, screen_top_left.y});
		
		// Concatenate the scaling matrix to the rotation matrix
		auto combined_matrix = rotation_matrix * scaling_matrix;
		
        d2d1_render_target->SetTransform(combined_matrix);
	    
		generic_brush->SetColor({color.red, color.green, color.blue, color.alpha});
		
		d2d1_render_target->FillRectangle(&rect, generic_brush.Get());
		
		d2d1_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
	}
	
	void draw_circle(ColorParams color, coordinates<float> location, float radius, bool zoom_independent)
	{
		if (!zoom_independent)
		{
			radius *= zoom_level;
		}
		
		location = world_to_screen(location);
		D2D1_ELLIPSE shape = D2D1::Ellipse({location.x, location.y}, radius, radius/2);
	    
		generic_brush->SetColor({color.red, color.green, color.blue, color.alpha});
	
		// Draw the ellipse
		d2d1_render_target->FillEllipse(&shape, generic_brush.Get());
	}

	void draw_unit_shadow(coordinates<float> screen_location, float radius)
	{
		D2D1_POINT_2F gradientCenter = D2D1::Point2F(screen_location.x, screen_location.y); // Center of the gradient
	    D2D1_POINT_2F gradientOriginOffset = D2D1::Point2F(0, 0); // No offset
	    
	    FLOAT gradientRadiusX = radius * zoom_level; // X radius
	    FLOAT gradientRadiusY = radius * 0.667f * zoom_level; // Y radius
	    
	    shadow_rotation = D2D1::Matrix3x2F::Rotation(-45.0f, gradientCenter);
        d2d1_render_target->SetTransform(shadow_rotation);
        
        pRadialGradientBrush->SetCenter(gradientCenter);
        pRadialGradientBrush->SetGradientOriginOffset(gradientOriginOffset);
        pRadialGradientBrush->SetRadiusX(gradientRadiusX);
        pRadialGradientBrush->SetRadiusY(gradientRadiusY);
        
	     d2d1_render_target->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(screen_location.x, screen_location.y), 60, 30),
            pRadialGradientBrush
        );
        
        d2d1_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
	}
	
	void draw_facing_helper(const coordinates<float> target)
	{
		const coordinates<float> center = {get_resolution().x/2, get_resolution().y/2};
		const coordinates<float> direction = target.towards(center);
		coordinates<float> topVertex = center + (direction * -48.0f);
	    float height = 18.0f;
	    float width = 8.0f;
	    
	    // Calculate the base center
	    coordinates<float> baseCenter;
	    baseCenter.x = topVertex.x + height * direction.x;
	    baseCenter.y = topVertex.y + height * direction.y;
	
	    // Calculate the offsets for the base vertices
	    coordinates<float> perpDirection = {-direction.y, direction.x};
	
	    coordinates<float> baseLeft, baseRight;
	    baseLeft.x = baseCenter.x + width/2 * perpDirection.x;
	    baseLeft.y = baseCenter.y + width/2 * perpDirection.y;
	
	    baseRight.x = baseCenter.x - width/2 * perpDirection.x;
	    baseRight.y = baseCenter.y - width/2 * perpDirection.y;
	    
	    D2D1_POINT_2F A = {topVertex.x, topVertex.y};
	    D2D1_POINT_2F B = {baseLeft.x, baseLeft.y};
	    D2D1_POINT_2F C = {baseRight.x, baseRight.y};
    
	    // Draw the triangle
	    d2d1_render_target->DrawLine(A, B, sc_brush.Get(), 2.0f);
	    d2d1_render_target->DrawLine(B, C, sc_brush.Get(), 2.0f);
	    d2d1_render_target->DrawLine(C, A, sc_brush.Get(), 2.0f);
	}
	
	void draw_sprite_to_map(ID2D1Bitmap* bitmap, coordinates<float> dimensions, coordinates<float> map_location, float z)
	{
		draw_sprite_to_map(bitmap, dimensions, map_location, coordinates<float>(), coordinates<float>(), z);
	}
	
	void draw_sprite_to_map(ID2D1Bitmap* bitmap, coordinates<float> dimensions, coordinates<float> map_location, coordinates<float> atlas_location, coordinates<float> atlas_dimensions, float z)
	{
	    coordinates<float> resolution = get_resolution();
	    coordinates<float> screen_center = { resolution.x / 2.0f, resolution.y / 2.0f };
	
	    // Calculate screen coordinates from world coordinates
	    coordinates<float> screen_coords = world_to_screen(map_location, 0); // Assuming z = 0 for simplicity
	
	    // Check if the sprite is within the visible screen area
	    if (screen_coords.x - dimensions.x * zoom_level > resolution.x || screen_coords.x + dimensions.x * zoom_level < 0)
	    {
	        return;
	    }
	
	    if (screen_coords.y - dimensions.y * zoom_level > resolution.y || screen_coords.y + dimensions.y * zoom_level < 0)
	    {
	        return;
	    }
	
	    // Adjust dimensions for zoom level
	    coordinates<float> zoomed_dimensions = dimensions;
	    zoomed_dimensions.x *= zoom_level;
	    zoomed_dimensions.y *= zoom_level;
	
	    // Define the drawing rectangle
	    D2D1_RECT_F draw_rect = D2D1::RectF(
	        screen_coords.x - (zoomed_dimensions.x / 2), screen_coords.y - zoomed_dimensions.y + 18.0f - z, 
	        screen_coords.x + (zoomed_dimensions.x / 2), screen_coords.y + 18.0f - z
	    );
	
	    // Draw the shadow
	    draw_unit_shadow({draw_rect.left + zoomed_dimensions.x / 1.667f, z + draw_rect.bottom - zoomed_dimensions.y / 5.67f}, 36.0f);
	
	    // Flip the atlas if needed
	    if (atlas_dimensions.x < 0)
	    {
	        D2D1_POINT_2F center = D2D1::Point2F(screen_coords.x, screen_coords.y - (zoomed_dimensions.y / 2.0f));
	        atlas_dimensions.x = std::abs(atlas_dimensions.x);
	        D2D1_MATRIX_3X2_F transform = D2D1::Matrix3x2F::Translation(-center.x, -center.y) *
	                                      D2D1::Matrix3x2F::Scale(-1.0f, 1.0f) *
	                                      D2D1::Matrix3x2F::Translation(center.x, center.y);
	        d2d1_render_target->SetTransform(transform);
	    }
	
	    // Define the atlas rectangle
	    D2D1_RECT_F atlas_rect = D2D1::RectF(
	        atlas_location.x, atlas_location.y,
	        atlas_location.x + atlas_dimensions.x, atlas_location.y + atlas_dimensions.y
	    );
	
	    // Draw the bitmap
	    if (atlas_dimensions)
	    {
	        d2d1_render_target->DrawBitmap(
	            bitmap,
	            draw_rect, 
	            1.0f,
	            default_interp_mode, 
	            &atlas_rect);
	    }
	    else
	    {
	        d2d1_render_target->DrawBitmap(
	            bitmap,
	            draw_rect, 
	            1.0f,
	            default_interp_mode, 
	            NULL);
	    }
	
	    // Reset the transform
	    d2d1_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
	}
	
	void draw_map_tile(ID2D1Bitmap* bitmap, coordinates<float> dimensions, coordinates<float> map_location, float tile_size)
	{
	    coordinates<float> resolution = get_resolution();
	    coordinates<float> screen_center = { resolution.x / 2.0f, resolution.y / 2.0f };
	
	    // Calculate screen coordinates from world coordinates
	    coordinates<float> screen_coords = world_to_screen(map_location, 0); // Assuming z = 0 for simplicity
	
	    // Check if the tile is within the visible screen area
	    if (screen_coords.x - dimensions.x * zoom_level > resolution.x || screen_coords.x + dimensions.x * zoom_level < 0)
	    {
	        return;
	    }
	
	    if (screen_coords.y - dimensions.y * zoom_level > resolution.y || screen_coords.y + dimensions.y * zoom_level < 0)
	    {
	        return;
	    }
	
	    // Adjust dimensions for zoom level
	    coordinates<float> zoomed_dimensions = dimensions;
	    zoomed_dimensions.x *= zoom_level;
	    zoomed_dimensions.y *= zoom_level;
	
	    // Define the drawing rectangle
	    D2D1_RECT_F draw_rect = D2D1::RectF(
	        screen_coords.x - (zoomed_dimensions.x / 2), screen_coords.y - zoomed_dimensions.y, 
	        screen_coords.x + (zoomed_dimensions.x / 2), screen_coords.y
	    );
	
	    // Draw the bitmap
	    d2d1_render_target->DrawBitmap(
	        bitmap,
	        draw_rect,
	        1.0f,
	        default_interp_mode,
	        NULL);
	}
	
	ID2D1Bitmap* create_bitmap_from_file(std::wstring filepath)
	{
		std::string fail_message = "Cannot create bitmap from file: ";
		if (!wic_factory.Get())
		{
			check_hr(fail_message + "WIC Factory invalid.", MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 70001));
		}
		
		// std::wcout << L"Filepath: " << filepath << std::endl;
		IWICBitmapDecoder* decoder;
		HRESULT hr;
		hr = wic_factory->CreateDecoderFromFilename(
				filepath.c_str(),
				nullptr,
				GENERIC_READ,
				WICDecodeMetadataCacheOnLoad,
				&decoder);
		check_hr(fail_message + "decoder creation failed\n"+wstring_to_string(filepath), hr);
				
		IWICBitmapFrameDecode* frame = nullptr;
		hr = decoder->GetFrame(0, &frame);
		check_hr(fail_message + "could not get frame.", hr);
		
		IWICFormatConverter* converter = nullptr;
		hr = wic_factory->CreateFormatConverter(&converter);
		check_hr(fail_message + "format converter creation failed.", hr);
		
		hr = converter->Initialize(
				frame,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				nullptr,
				0.f,
				WICBitmapPaletteTypeMedianCut);
		check_hr(fail_message + "failed to initialize format converter.", hr);
		
		ID2D1Bitmap* bitmap = nullptr;
		hr = d2d1_render_target->CreateBitmapFromWicBitmap(
				converter,
				nullptr,
				&bitmap);
		check_hr(fail_message + "render target could not create bitmap from wic type.", hr);
		
		return bitmap;
	}
	
	IWICBitmap* create_wicmap_from_file(const std::wstring &filepath)
	{
		std::string fail_message = "Cannot create bitmap from file: ";
		
	    if (!wic_factory) {
	        check_hr(fail_message + "WIC Factory invalid.", MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 70001));
	    }
	
	    IWICBitmapDecoder* decoder = nullptr;
	    HRESULT hr = wic_factory->CreateDecoderFromFilename(
	        filepath.c_str(),
	        nullptr,
	        GENERIC_READ,
	        WICDecodeMetadataCacheOnLoad,
	        &decoder);
	    check_hr(fail_message + "decoder creation failed.", hr);
	
	    IWICBitmapFrameDecode* frame = nullptr;
	    hr = decoder->GetFrame(0, &frame);
	    check_hr(fail_message + "could not get frame.", hr);
	
	    IWICBitmap* wicBitmap = nullptr;
	    hr = wic_factory->CreateBitmapFromSource(
	        frame,
	        WICBitmapCacheOnLoad,
	        &wicBitmap);
	    check_hr(fail_message + "could not create IWICBitmap.", hr);
	
	    // Release resources
	    if (frame) frame->Release();
	    if (decoder) decoder->Release();
	
	    return wicBitmap;
	}
	
	void prerender()
	{
		d2d1_render_target->BeginDraw();
		d2d1_render_target->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		return;
	}

	bool initialize()
	{
		win_compat::Window& window = win_compat::Window::instance();

		DestroyObserver = std::shared_ptr<WO_DestroyObserver>(new WO_DestroyObserver());
		window.add_observer(WM_DESTROY, DestroyObserver);

		HRESULT hr;

		std::println("Beginning DXGI initialization...");

		hr = CreateDXGIFactory(__uuidof(IDXGIFactory), &dxgi_factory);
		if (FAILED(hr))
		{
			std::println("Failed to create DXGI factory...");
			return false;
		}

		ComPtr<IDXGIAdapter> running_adapter;
		UINT adapter_index = 0;
		SIZE_T max_gpu_memory = 0;
		DXGI_ADAPTER_DESC adapter_desc;
		while (dxgi_factory->EnumAdapters(adapter_index, running_adapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
		{
			hr = running_adapter->GetDesc(&adapter_desc);
			if (SUCCEEDED(hr))
			{
				if (adapter_desc.DedicatedVideoMemory > max_gpu_memory)
				{
					dxgi_adapter = running_adapter;
				}
			}
			adapter_index++;
		}
		dxgi_adapter->GetDesc(&adapter_desc);
		std::wcout << L"Using " << adapter_desc.Description << std::endl;

		UINT device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef GFX_DEBUG
		device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		ComPtr<ID3D11Device> d3d11_device;
		ComPtr<ID3D11DeviceContext> d3d11_context;
		D3D_FEATURE_LEVEL d3d11_feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };
		UINT d3d11_feature_levels_count = ARRAYSIZE(d3d11_feature_levels);
		hr = D3D11CreateDevice(
			dxgi_adapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			device_flags,
			d3d11_feature_levels,
			d3d11_feature_levels_count,
			D3D11_SDK_VERSION,
			&d3d11_device,
			nullptr,
			&d3d11_context);

		if (FAILED(hr))
		{
			check_hr("Failed to create D3D11 device", hr);
		}

		coordinates<float> resolution = get_resolution();
		DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
		swap_chain_desc.BufferCount = 2;
		swap_chain_desc.Width = static_cast<UINT>(resolution.x);
		swap_chain_desc.Height = static_cast<UINT>(resolution.y);
		swap_chain_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;
		swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
		swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		hr = dxgi_factory->CreateSwapChainForHwnd(d3d11_device.Get(), window.handle, &swap_chain_desc, nullptr, nullptr, &swap_chain);
		if (FAILED(hr))
		{
			std::println("Failed to create swap chain.");
			return false;
		}
		else
		{
			std::println("Swap chain initialized.");
		}

		hr = d3d11_device->QueryInterface(__uuidof(IDXGIDevice), &dxgi_device);

		std::println("Beginning Direct2D initialization...");

		D2D1_FACTORY_OPTIONS factory_options;
		ZeroMemory(&factory_options, sizeof(D2D1_FACTORY_OPTIONS));
		factory_options.debugLevel = D2D1_DEBUG_LEVEL_WARNING;
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &factory_options, &d2d1_factory);
		if (FAILED(hr))
		{
			std::println("Failed to create D2D Factory.");
			return false;
		}

		hr = d2d1_factory->CreateDevice(dxgi_device.Get(), &d2d1_device);
		if (FAILED(hr))
		{
			check_hr("Failed to create D2D device", hr);

			return false;
		}

		hr = d2d1_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d1_device_context);
		if (FAILED(hr))
		{
			std::println("Failed to create Direct2D device context.");
			return false;
		}

		RECT client_rect;
		GetClientRect(window.handle, &client_rect);

		hr = swap_chain->GetBuffer(0, IID_PPV_ARGS(&dxgi_surface));
		if (FAILED(hr))
		{
			std::println("Failed to get swap chain buffer.");
			return false;
		}
		
		ID2D1RenderTarget* new_render_target;
		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
		hr = d2d1_factory->CreateDxgiSurfaceRenderTarget(dxgi_surface.Get(), props, &new_render_target);
		if (FAILED(hr))
		{
			std::println("Failed to create DXGI surface.");
			return false;
		}
		d2d1_render_target = std::unique_ptr<ID2D1RenderTarget>(new_render_target);
		
		hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &black_brush);
		if (FAILED(hr))
		{
			std::println("Failed to create solid color brush.");
			return false;
		}
		
		hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkSlateBlue), &generic_brush);
		if (FAILED(hr))
		{
			std::println("Failed to create solid color brush.");
			return false;
		}
		
		hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Silver), &sc_brush);
		if (FAILED(hr))
		{
			std::println("Failed to create solid color brush.");
			return false;
		}
		
		std::println("Initializing render layers...");
		
		ID2D1BitmapRenderTarget* new_game_render_target;
		hr = d2d1_render_target->CreateCompatibleRenderTarget(d2d1_render_target->GetSize(), &new_game_render_target);
		check_hr("CreateCompatibleRenderTarget game_render_target", hr);
		game_render_target = std::unique_ptr<ID2D1BitmapRenderTarget>(new_game_render_target);
		
		ID2D1BitmapRenderTarget* new_ui_render_target;
		hr = d2d1_render_target->CreateCompatibleRenderTarget(d2d1_render_target->GetSize(), &new_ui_render_target);
		check_hr("CreateCompatibleRenderTarget ui_render_target", hr);
		ui_render_target = std::unique_ptr<ID2D1BitmapRenderTarget>(new_ui_render_target);
		
		std::println("Initializing image decoding...");
		
		hr = CoCreateInstance(
				CLSID_WICImagingFactory,
				nullptr,
				CLSCTX_INPROC_SERVER,
				IID_IWICImagingFactory,
				&wic_factory);
		if (FAILED(hr))
		{
			std::println("Failed to create imaging factory.");
			return false;
		}

		std::println("Direct2D initialized successfully.");
		
		std::println("Initializing DirectWrite...");
		
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), &dwrite_factory);
			
		std::cout << "Initializing graphical effects" << std::endl;
		
		d2d1_device_context->CreateEffect(CLSID_D2D1PointSpecular, &specular);
		
		D2D1_GRADIENT_STOP gradientStops[3];
		gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Black, 0.5f); // Opaque black at the center
		gradientStops[0].position = 0.0f;
		gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Black, 0.2f); // Opaque black at the center
		gradientStops[1].position = 0.8f;
		gradientStops[2].color = D2D1::ColorF(D2D1::ColorF::Black, 0.0f); // Transparent black at the edges
		gradientStops[2].position = 1.0f;

		hr = d2d1_render_target->CreateGradientStopCollection(
		    gradientStops,
		    3,
		    &pGradientStops
		);
	
	    // Create the radial gradient brush
	    hr = d2d1_render_target->CreateRadialGradientBrush(
	        D2D1::RadialGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, 0), 0, 0),
	        pGradientStops,
	        &pRadialGradientBrush
	    );
	    
		std::cout << "DirectX initialized successfully." << std::endl;
		
		return true;
	}

	void present(bool vsync)
	{
		check_hr("EndDraw", d2d1_render_target->EndDraw());
		swap_chain->Present(1, 0);
	}

	coordinates<float> get_resolution()
	{
		static bool cached = false;
		static coordinates<float> resolution;
		
		if (!cached)
		{
			win_compat::Window& window = win_compat::Window::instance();
			RECT client_rect;
			GetWindowRect(window.handle, &client_rect);
			resolution = { float(client_rect.right - client_rect.left), float(client_rect.bottom - client_rect.top) };
			cached = true;
		}
		
		return resolution;
	}
	
	D2D1_RECT_F get_resolution_rect()
	{
		coordinates<float> resolution = get_resolution();
		D2D1_RECT_F resolution_rect = D2D1::RectF(0.0f, 0.0f, resolution.x, resolution.y);
		return resolution_rect;
	}
}