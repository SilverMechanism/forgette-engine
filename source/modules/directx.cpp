module;
#include <dwrite_3.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <windows.h>
#include <wincodec.h>
#include <cassert>
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
ComPtr<ID2D1SolidColorBrush> sc_brush;
std::unique_ptr<ID2D1Bitmap> logo;

ComPtr<IWICBitmapDecoder> decoder;
ComPtr<IWICImagingFactory> wic_factory;
ComPtr<IWICFormatConverter> tga_converter;

ComPtr<IDWriteFactory5> dwrite_factory;
ComPtr<IDWriteTextFormat> text_format;
ComPtr<IDWriteTextLayout> text_layout;
ComPtr<IDWriteFontFile> old_english_font_file;
ComPtr<IDWriteFontFace> old_english_font_face;

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
	
	ID2D1RenderTarget* get_render_target()
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
	
	void draw_sprite_to_map(ID2D1Bitmap* bitmap, coordinates<float> dimensions, coordinates<float> map_location);
	// void draw_sprite_to_map(ID2D1Bitmap* bitmap, coordinates<float> dimensions, coordinates<float> map_location, coordinates<int> atlas_location, coordinates<int> atlas_size);
	void draw_map_tile(ID2D1Bitmap* bitmap, coordinates<float> dimensions, coordinates<float> map_location, float tile_size);

	coordinates<float> map_to_isometric(coordinates<float> map_location);
}

namespace ForgetteDirectX
{
	coordinates<float> render_viewpoint;
	coordinates<float> viewpoint_anchor;
	float zoom_level {1.0f};
	
	float get_zoom_level()
	{
		return zoom_level;
	}
	
	void set_zoom_level(float new_zoom_level)
	{
		zoom_level = new_zoom_level;
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
	
	coordinates<float> map_to_isometric(coordinates<float> map_location)
	{
		coordinates<float> iso_coords;
		iso_coords.x = (map_location.x + map_location.y) * (64)/36;
		iso_coords.y = (map_location.x - map_location.y) * (32)/36;
		return iso_coords;
	}
	
	void draw_sprite_to_map(ID2D1Bitmap* bitmap, coordinates<float> dimensions, coordinates<float> map_location)
	{
		coordinates<float> resolution = get_resolution();
		
		float dx = (map_location.x - render_viewpoint.x);
		float dy = (map_location.y - render_viewpoint.y);
		
		float x_on_window;
		float y_on_window;
		
		coordinates<float> isometric_coords;
		x_on_window = (dx + dy) * (64)/36 + (resolution.x/2);
		y_on_window = (dx - dy) * (32)/36 + (resolution.y/2);
		
		if (x_on_window-dimensions.x > resolution.x || x_on_window+dimensions.x < 0)
		{
			return;
		}
		
		if (y_on_window-dimensions.y > resolution.y || y_on_window+dimensions.y < 0)
		{
			return;
		}
		
		dimensions.y *= zoom_level;
		dimensions.x *= zoom_level;
		
		D2D1_RECT_F draw_rect = D2D1::RectF(
			x_on_window-(dimensions.x/2), y_on_window-dimensions.y, 
			x_on_window+(dimensions.x/2), y_on_window
		);
		
		d2d1_render_target->DrawBitmap(
			bitmap,
			draw_rect, 
			1.0f,
			default_interp_mode, 
			NULL);
	}
	
	void draw_map_tile(ID2D1Bitmap* bitmap, coordinates<float> dimensions, coordinates<float> map_location, float tile_size)
	{
		coordinates<float> resolution = get_resolution();
		
		float dx = (map_location.x - render_viewpoint.x);
		float dy = (map_location.y - render_viewpoint.y);
		
		float x_on_window;
		float y_on_window;
		
		coordinates<float> isometric_coords;
		x_on_window = (dx + dy) * (dimensions.x/2)/tile_size;
		y_on_window = (dx - dy) * (dimensions.y/2)/tile_size;
		
		if (x_on_window-dimensions.x > resolution.x || x_on_window+dimensions.x < 0)
		{
			return;
		}
		
		if (y_on_window-dimensions.y > resolution.y || y_on_window+dimensions.y < 0)
		{
			return;
		}
		
		dimensions.y *= zoom_level;
		dimensions.x *= zoom_level;
			
		D2D1_RECT_F draw_rect =
			D2D1::RectF
			(
				x_on_window, y_on_window, 
				x_on_window+dimensions.x, y_on_window+dimensions.y
			);
		
		d2d1_render_target->DrawBitmap(
			bitmap,
			draw_rect, 
			1.0f,
			default_interp_mode, 
			NULL);
	}
	
	void draw_entity_rect(coordinates<float> dimensions, coordinates<float> map_location, ProjectionMode projection)
	{
		coordinates<float> resolution = get_resolution();
		
		float dx = (map_location.x - render_viewpoint.x);
		float dy = (map_location.y + render_viewpoint.y);
		
		if (projection == ProjectionMode::Ratio2_1)
		{
			dy *= 0.5;
		}
		
		float x_on_window = (dx + (resolution.x / 2));
		float y_on_window = (dy + (resolution.y / 2));
		
		D2D1_RECT_F draw_rect = D2D1::RectF(x_on_window-(dimensions.x/2), y_on_window-dimensions.y, x_on_window+(dimensions.x/2), y_on_window);
		
		d2d1_render_target->FillRectangle(draw_rect, sc_brush.Get());
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

		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr, &d2d1_factory);
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

		hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Silver), &sc_brush);
		if (FAILED(hr))
		{
			std::println("Failed to create solid color brush.");
			return false;
		}
		
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
			
		std::string studio_title = "SILVER MECHANISM";
		
		dwrite_factory->CreateTextFormat(
			L"Old English Text MT",
			nullptr,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			72.f,
			L"en-US",
			&text_format);
			
		dwrite_factory->CreateTextLayout(
			L"SILVER MECHANISM STUDIO",
			static_cast<UINT32>(studio_title.length()),
			text_format.Get(),
			1000.f,
			1000.f,
			&text_layout);
		
		std::cout << "DirectX initialized successfully." << std::endl;
		
		return true;
	}

	void present(bool vsync)
	{
		d2d1_render_target->EndDraw();

		swap_chain->Present(1, 0);
	}

	coordinates<float> get_resolution()
	{
		win_compat::Window& window = win_compat::Window::instance();
		RECT client_rect;
		GetWindowRect(window.handle, &client_rect);

		return { float(client_rect.right - client_rect.left), float(client_rect.bottom - client_rect.top) };
	}
	
	D2D1_RECT_F get_resolution_rect()
	{
		coordinates<float> resolution = get_resolution();
		D2D1_RECT_F resolution_rect = D2D1::RectF(0.0f, 0.0f, resolution.x, resolution.y);
		return resolution_rect;
	}
}