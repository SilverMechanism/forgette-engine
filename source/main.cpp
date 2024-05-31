module;
#include <windows.h>
#include <cstdio>
#include "luajit/lua.hpp"
export module main;
import forgette;
import windows;
import input;
import lua_interop;
import core;
import std;

MSG message;

static bool bQuit = false;
static int _exit_code = 0;

void Quit(int exit_code);

void QuitNormally();

void PlayIntro();

class WO_CloseObserver : public win_compat::WindowObserver
{
	virtual bool on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override
	{
		QuitNormally();

		return true;
	}
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
int argc = __argc;
wchar_t** argv = __wargv;

#ifdef _DEBUG
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
	{
		AllocConsole();
	}

	FILE* fp;
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
#endif
	{
	    wchar_t buffer[MAX_PATH];
	    DWORD result = GetModuleFileNameW(NULL, buffer, MAX_PATH);
	    
	    if (result == 0) {
	        // Handle error
	        DWORD error = GetLastError();
	        std::wcerr << L"Error getting module file name: " << error << std::endl;
	        return 1;
	    }
	
	    std::wstring exe_path(buffer);
	    set_exe_path(exe_path);
	}
	
	win_compat::Window &window = win_compat::Window::instance();
	win_compat::initialize(hInstance);
	
	ShowWindow(window.handle, nCmdShow);
	UpdateWindow(window.handle);
	SetCapture(window.handle);
	
	// std::println("Initializing engine...");
	Forgette::Engine* engine = new Forgette::Engine();
	
	if (!engine->healthy)
	{
		// std::println("Fatal Error: GFX failed to initialize.");
		Quit(1);
	}
	else
	{
		// std::println("GFX successfully initialized.");
	}
	
	set_engine(engine);

	std::shared_ptr<WO_CloseObserver> CloseObserver = std::shared_ptr<WO_CloseObserver>(new WO_CloseObserver());
	window.add_observer(WM_CLOSE, CloseObserver);

	input::BindToWindow();
	input::AddInputBinding(VK_ESCAPE, input::KeyEventType::key_down, &QuitNormally, 0);
	
	bool show_cursor = false;
    for (int i = 1; i < argc; ++i)
    {
        if (wcscmp(argv[i], L"-show_cursor") == 0)
        {
            show_cursor = true;
            break;
        }
    }
	ShowCursor(show_cursor);
	
	LuaInterop::register_functions(engine->lua_manager.get()->lua_state);
	
	std::wstring map_script_path = engine->get_application_dir() + L"\\blood_fields.tofs";
	engine->load_game_map(map_script_path);
	
	// InvalidateRect(win_compat::Window::instance().handle, NULL, TRUE);

	while (!bQuit)
	{
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				bQuit = true;
			}

			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		engine->loop();
	}

	// std::println("Goodbye.");
	FreeConsole();
	return _exit_code;
}

void Quit(int exit_code)
{
	_exit_code = exit_code;
	win_compat::Window& window = win_compat::Window::instance();
	DestroyWindow(window.handle);
	PostQuitMessage(_exit_code);
	bQuit = true;
}

void QuitNormally()
{
	Quit(0);
}