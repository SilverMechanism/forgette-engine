module;
#include <windows.h>
#include <hidusage.h>
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif
export module windows;
import std;
import core;

export namespace win_compat
{
	class WindowObserver
	{
	public:
		virtual bool on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	};

	class Window
	{
	private:
		std::map<UINT, std::vector<std::weak_ptr<WindowObserver>>> msg_observer_map;

	public:
		HWND handle;

		static Window& instance()
		{
			static Window instance;
			return instance;
		}

		Window(const Window&) = delete;
		void operator=(const Window&) = delete;

		void add_observer(UINT uMsg, std::weak_ptr<WindowObserver> observer)
		{
			msg_observer_map[uMsg].push_back(observer);
		}

		bool notify_observers(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (msg_observer_map.find(uMsg) != msg_observer_map.end())
			{
				auto& observers = msg_observer_map[uMsg];
				for (auto it = observers.begin(); it != observers.end();)
				{
					auto observer = it->lock();
					if (observer)
					{
						if (observer->on_message(hwnd, uMsg, wParam, lParam))
							return true;
						++it;
					}
					else
					{
						it = observers.erase(it);
					}
				}
			}
			return false;
		}

		void initialize_mouse();

	protected:
		Window() {}
	};
}

using namespace win_compat;

void Window::initialize_mouse()
{
	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = handle;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (Window::instance().notify_observers(hwnd, uMsg, wParam, lParam))
		return 0;

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

export namespace win_compat
{
	void initialize(HINSTANCE hInstance)
	{
		const wchar_t CLASS_NAME[] = L"GameWindow";
		WNDCLASSW wc = {};
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = CLASS_NAME;
		RegisterClassW(&wc);

		HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"Just Another Forgette Application", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
		Window::instance().handle = hwnd;
		Window::instance().initialize_mouse();
	}

	std::map<UINT, std::string> keyCodeMap
	{
	{0x0001, "Left mouse button"},
	{0x0002, "Right mouse button"},
	{0x0003, "CTRL+BREAK processing"},
	{0x0008, "BACKSPACE key"},
	{0x0009, "TAB key"},
	{0x000C, "CLEAR key"},
	{0x000D, "ENTER key"},
	{0x0010, "SHIFT key"},
	{0x0011, "CTRL key"},
	{0x0012, "ALT key"},
	{0x0013, "PAUSE key"},
	{0x0014, "CAPS LOCK key"},
	{0x001B, "ESC key"},
	{0x0020, "SPACEBAR"},
	{0x0021, "PAGE UP key"},
	{0x0022, "PAGE DOWN key"},
	{0x0023, "END key"},
	{0x0024, "HOME key"},
	{0x0025, "LEFT ARROW key"},
	{0x0026, "UP ARROW key"},
	{0x0027, "RIGHT ARROW key"},
	{0x0028, "DOWN ARROW key"},
	{0x0029, "SELECT key"},
	{0x002A, "PRINT key"},
	{0x002B, "EXECUTE key"},
	{0x002C, "PRINT SCREEN key"},
	{0x002D, "INS key"},
	{0x002E, "DEL key"},
	{0x002F, "HELP key"},
	{0x0030, "0 key"},
	{0x0031, "1 key"},
	{0x0032, "2 key"},
	{0x0033, "3 key"},
	{0x0034, "4 key"},
	{0x0035, "5 key"},
	{0x0036, "6 key"},
	{0x0037, "7 key"},
	{0x0038, "8 key"},
	{0x0039, "9 key"},
	{0x0041, "A key"},
	{0x0042, "B key"},
	{0x0043, "C key"},
	{0x0044, "D key"},
	{0x0045, "E key"},
	{0x0046, "F key"},
	{0x0047, "G key"},
	{0x0048, "H key"},
	{0x0049, "I key"},
	{0x004A, "J key"},
	{0x004B, "K key"},
	{0x004C, "L key"},
	{0x004D, "M key"},
	{0x004E, "N key"},
	{0x004F, "O key"},
	{0x0050, "P key"},
	{0x0051, "Q key"},
	{0x0052, "R key"},
	{0x0053, "S key"},
	{0x0054, "T key"},
	{0x0055, "U key"},
	{0x0056, "V key"},
	{0x0057, "W key"},
	{0x0058, "X key"},
	{0x0059, "Y key"},
	{0x005A, "Z key"},
	{0x005B, "Left Windows key (Microsoft Natural Keyboard)"},
	{0x005C, "Right Windows key (Microsoft Natural Keyboard)"},
	{0x005D, "Applications key (Microsoft Natural Keyboard)"},
	{0x005F, "Computer Sleep key"},
	{0x0060, "Numeric keypad 0 key"},
	{0x0061, "Numeric keypad 1 key"},
	{0x0062, "Numeric keypad 2 key"},
	{0x0063, "Numeric keypad 3 key"},
	{0x0064, "Numeric keypad 4 key"},
	{0x0065, "Numeric keypad 5 key"},
	{0x0066, "Numeric keypad 6 key"},
	{0x0067, "Numeric keypad 7 key"},
	{0x0068, "Numeric keypad 8 key"},
	{0x0069, "Numeric keypad 9 key"},
	{0x006A, "Multiply key"},
	{0x006B, "Add key"},
	{0x006C, "Separator key"},
	{0x006D, "Subtract key"},
	{0x006E, "Decimal key"},
	{0x006F, "Divide key"},
	{0x0070, "F1 key"},
	{0x0071, "F2 key"},
	{0x0072, "F3 key"},
	{0x0073, "F4 key"},
	{0x0074, "F5 key"},
	{0x0075, "F6 key"},
	{0x0076, "F7 key"},
	{0x0077, "F8 key"},
	{0x0078, "F9 key"},
	{0x0079, "F10 key"},
	{0x007A, "F11 key"},
	{0x007B, "F12 key"},
	{0x007C, "F13 key"},
	{0x007D, "F14 key"},
	{0x007E, "F15 key"},
	{0x007F, "F16 key"},
	{0x0080, "F17 key"},
	{0x0081, "F18 key"},
	{0x0082, "F19 key"},
	{0x0083, "F20 key"},
	{0x0084, "F21 key"},
	{0x0085, "F22 key"},
	{0x0086, "F23 key"},
	{0x0087, "F24 key"},
	{0x0090, "NUM LOCK key"},
	{0x0091, "SCROLL LOCK key"},
	{0x00A0, "Left SHIFT key"},
	{0x00A1, "Right SHIFT key"},
	{0x00A2, "Left CONTROL key"},
	{0x00A3, "Right CONTROL key"},
	{0x00A4, "Left MENU key"},
	{0x00A5, "Right MENU key"}
	};
}