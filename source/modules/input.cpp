module;
#include <windows.h>
#include <assert.h>
export module input;
import windows;
import std;
import core;
import directx;

export namespace input
{	
	enum class ModifierKey
	{
		none = 0x0000,
		shift = 0x0010,
		ctrl = 0x0011,
		alt = 0x0012
	};

	enum class KeyEventType
	{
		key_down,
		key_up
	};

	enum class InputState
	{
		inactive,
		active
	};

	// Classes ---

	struct InputBind
	{
		InputState state = InputState::inactive;
		std::vector<std::function<void()>> key_down_bindings;
		std::vector<std::function<void()>> key_up_bindings;
	};

	int mouse_relative_x = 0;
	int mouse_relative_y = 0;
}

static input::ModifierKey ActiveModifierKey = input::ModifierKey::none;
bool bShiftHeld = false;
bool bCtrlHeld = false;
bool bAltHeld = false;

static std::vector<input::InputBind> input_binds(255);
static std::unordered_map<std::string, int> key_bindings;

coordinates<float> cursor_location;

export namespace input
{
	std::map<int, InputBind> active_down_binds;
	
	coordinates<float> get_cursor_screen_location();
	coordinates<float> get_cursor_map_location();
	
	// Returns index of binding, -1 if not found
	int FindInputBinding(int MatchingKey, KeyEventType MatchingKeyEvent)
	{
		//for (int i = 0; i < InputBindings.size(); i++)
		//{
		//	if (InputBindings[i].key_code == MatchingKey && InputBindings[i].KeyEvent == MatchingKeyEvent)
		//	{
		//		if (InputBindings[i].Modifier == ActiveModifierKey || InputBindings[i].Modifier == ModifierKey::none)
		//		{
		//			return i;
		//		}
		//	}
		//}

		return -1;
	}

	int FindInputBinding(int KeyToSearch, ModifierKey KeyModifier, KeyEventType MatchingKeyEvent)
	{
		//for (int i = 0; i < InputBindings.size(); i++)
		//{
		//	if (InputBindings[i].key_code == KeyToSearch && InputBindings[i].Modifier == KeyModifier && InputBindings[i].KeyEvent == MatchingKeyEvent)
		//	{
		//		return i;
		//	}
		//}

		return -1;
	}

	bool AddInputBinding(int key, KeyEventType event_type, std::function<void()> function, int priority)
	{
		assert(key < 254); // Exceeds Windows virtual key codes

		if (event_type == KeyEventType::key_down)
		{
			if (input_binds[key].key_down_bindings.size() == 0 || priority > input_binds[key].key_down_bindings.size())
			{
				input_binds[key].key_down_bindings.push_back(function);
			}
			else
			{
				input_binds[key].key_down_bindings.insert(input_binds[key].key_down_bindings.begin() + priority, function);
			}
		}
		else if (event_type == KeyEventType::key_up)
		{
			if (input_binds[key].key_up_bindings.size() == 0 || priority > input_binds[key].key_up_bindings.size())
			{
				input_binds[key].key_up_bindings.push_back(function);
			}
			else
			{
				input_binds[key].key_up_bindings.insert(input_binds[key].key_down_bindings.begin() + priority, function);
			}
		}
		else
		{
			return false;
		}

		return true;
	}
	
	bool AddInputBinding(std::string name, KeyEventType event_type, std::function<void()> function, int priority)
	{
		int key = key_bindings[name];
		
		assert(key < 254); // Exceeds Windows virtual key codes

		if (event_type == KeyEventType::key_down)
		{
			if (input_binds[key].key_down_bindings.size() == 0 || priority > input_binds[key].key_down_bindings.size())
			{
				input_binds[key].key_down_bindings.push_back(function);
			}
			else
			{
				input_binds[key].key_down_bindings.insert(input_binds[key].key_down_bindings.begin() + priority, function);
			}
		}
		else if (event_type == KeyEventType::key_up)
		{
			if (input_binds[key].key_up_bindings.size() == 0 || priority > input_binds[key].key_up_bindings.size())
			{
				input_binds[key].key_up_bindings.push_back(function);
			}
			else
			{
				input_binds[key].key_up_bindings.insert(input_binds[key].key_down_bindings.begin() + priority, function);
			}
		}
		else
		{
			return false;
		}

		return true;
	}
	
	bool map_key(std::string name, int key, bool overwrite)
	{
		assert(key < 255);
		
		if (!overwrite && (key_bindings.find(name) != key_bindings.end()))
		{
			return false;
		}
		
		key_bindings[name] = key;
		
		return true;
	}
}

class WO_KeyDownObserver : public win_compat::WindowObserver
{
	virtual bool on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override
	{
		int key = static_cast<int>(wParam);

		if (uMsg == WM_KEYUP)
		{
			if (input_binds[key].state == input::InputState::active)
			{
				if (input_binds[key].key_up_bindings.size())
				{
					input_binds[key].key_up_bindings[0]();
				}
				input_binds[key].state = input::InputState::inactive;

				auto it = input::active_down_binds.find(key);
				if (it != input::active_down_binds.end())
				{
					input::active_down_binds.erase(it);
				}
			}
		}
		else if (uMsg == WM_KEYDOWN)
		{
			if (!input_binds[key].key_down_bindings.size())
			{
				return true;
			}

			if (input_binds[key].state == input::InputState::inactive)
			{
				input_binds[key].state = input::InputState::active;
				input::active_down_binds[key] = input_binds[key];
			}
		}

		return true;
	}
};

class WO_RawInputObserver : public win_compat::WindowObserver
{
     const bool debug_print = false;
     
	virtual bool on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override
	{
		switch (uMsg)
		{
		case WM_INPUT:
		{
			UINT dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];
	
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
	
			RAWINPUT* raw_input = (RAWINPUT*)lpb;
	
			if (raw_input->header.dwType == RIM_TYPEMOUSE)
			{
				input::mouse_relative_x = raw_input->data.mouse.lLastX;
				input::mouse_relative_y = raw_input->data.mouse.lLastY;
	               
	               if (debug_print)
	               {
	                    std::cout << "Mouse last X: " << input::mouse_relative_x << std::endl;
				     	std::cout << "Mouse last Y: " << input::mouse_relative_y << std::endl;
	               }
			}
			
			POINT absolute_position;
			if (GetCursorPos(&absolute_position))
			{
				if (debug_print)
				{
					std::cout << "[MOUSE ABSOLUTE]\nMouse absolute X: " << absolute_position.x << std::endl << "Mouse absolute Y: " << absolute_position.y << std::endl << std::endl;
				}
				
				ScreenToClient(hwnd, &absolute_position);
				if (debug_print)
				{
					std::cout << "[MOUSE RELATIVE]\nMouse relative X: " << absolute_position.x << std::endl << "Mouse relative Y: " << absolute_position.y << std::endl << std::endl;
				}
				
				cursor_location = {static_cast<float>(absolute_position.x), static_cast<float>(absolute_position.y)};
				
				if (GetForegroundWindow() == win_compat::Window::instance().handle)
				{
				    coordinates<int> resolution = ForgetteDirectX::get_resolution();
				    int center_x = resolution.x / 2;
				    int center_y = resolution.y / 2;
				
				    float dx = cursor_location.x - center_x;
				    float dy = cursor_location.y - center_y;
				    float distance = std::sqrt(dx * dx + dy * dy);
				
				    const float max_distance = 100.0f; // Circle radius
				
				    if (distance > max_distance)
				    {
				        float ratio = max_distance / distance;
				        int clamped_x = static_cast<int>(center_x + dx * ratio);
				        int clamped_y = static_cast<int>(center_y + dy * ratio);
				        SetCursorPos(clamped_x, clamped_y);
				    }
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
			if (input_binds[0x0001].key_down_bindings.size())
			{
				input_binds[0x0001].key_down_bindings[0]();
			}

			if (input_binds[0x0001].state == input::InputState::inactive)
			{
				input_binds[0x0001].state = input::InputState::active;
			}
			break;
		case WM_RBUTTONDOWN:
			if (input_binds[0x0002].key_down_bindings.size())
			{
				input_binds[0x0002].key_down_bindings[0]();
			}

			if (input_binds[0x0002].state == input::InputState::inactive)
			{
				input_binds[0x0002].state = input::InputState::active;
			}
			break;
		case WM_LBUTTONUP:
			if (input_binds[0x0001].state == input::InputState::active)
			{
				if (input_binds[0x0001].key_up_bindings.size())
				{
					input_binds[0x0001].key_up_bindings[0]();
				}
				input_binds[0x0001].state = input::InputState::inactive;
			}
			break;
		case WM_RBUTTONUP:
			if (input_binds[0x0002].state == input::InputState::active)
			{
				if (input_binds[0x0002].key_up_bindings.size())
				{
					input_binds[0x0002].key_up_bindings[0]();
				}
				input_binds[0x0002].state = input::InputState::inactive;
			}
			break;
		default:
			break;
		}
		return true;
	}
};

void SetActiveModifierKey(int KeyCode, input::KeyEventType EventType)
{
	if (EventType == input::KeyEventType::key_down && static_cast<int>(ActiveModifierKey) == KeyCode) { return; };

	switch (KeyCode)
	{
	case static_cast<int>(input::ModifierKey::ctrl):
		bCtrlHeld = (EventType == input::KeyEventType::key_down);
		break;
	case static_cast<int>(input::ModifierKey::alt):
		bAltHeld = (EventType == input::KeyEventType::key_down);
		break;
	case static_cast<int>(input::ModifierKey::shift):
		bShiftHeld = (EventType == input::KeyEventType::key_down);
		break;
	default:
		return;
	}

	// If this is our only held key
	int ModifierState = bCtrlHeld + bAltHeld + bShiftHeld;
	if (ModifierState == 1)
	{
		if (bCtrlHeld)
		{
			ActiveModifierKey = input::ModifierKey::ctrl;
		}
		else if (bAltHeld)
		{
			ActiveModifierKey = input::ModifierKey::alt;
		}
		else
		{
			ActiveModifierKey = input::ModifierKey::shift;
		}
	}
	else if (ModifierState == 0)
	{
		ActiveModifierKey = input::ModifierKey::none;
	}

	return;
}

void OnShiftDown()
{
	SetActiveModifierKey(0x0010, input::KeyEventType::key_down);
}

void OnShiftUp()
{
	SetActiveModifierKey(0x0010, input::KeyEventType::key_up);
}

void OnCtrlDown()
{
	SetActiveModifierKey(0x0011, input::KeyEventType::key_down);
}

void OnCtrlUp()
{
	SetActiveModifierKey(0x0011, input::KeyEventType::key_up);
}

void OnAltDown()
{
	SetActiveModifierKey(0x0012, input::KeyEventType::key_down);
}

void OnAltUp()
{
	SetActiveModifierKey(0x0012, input::KeyEventType::key_up);
}

static std::shared_ptr<WO_KeyDownObserver> KeyDownObserver;
static std::shared_ptr<WO_RawInputObserver> RawInputObserver;

export namespace input
{
	void BindToWindow()
	{
		KeyDownObserver = std::make_shared<WO_KeyDownObserver>();

		win_compat::Window& window = win_compat::Window::instance();
		window.add_observer(WM_KEYDOWN, KeyDownObserver);
		window.add_observer(WM_KEYUP, KeyDownObserver);

		RawInputObserver = std::make_shared<WO_RawInputObserver>();
		window.add_observer(WM_INPUT, RawInputObserver);
		
		window.add_observer(WM_LBUTTONDOWN, RawInputObserver);
		window.add_observer(WM_RBUTTONDOWN, RawInputObserver);
		
		window.add_observer(WM_LBUTTONUP, RawInputObserver);
		window.add_observer(WM_RBUTTONUP, RawInputObserver);

		AddInputBinding(VK_SHIFT, KeyEventType::key_down, OnShiftDown, 0);
		AddInputBinding(VK_SHIFT, KeyEventType::key_up, OnShiftUp, 0);

		AddInputBinding(VK_CONTROL, KeyEventType::key_down, OnCtrlDown, 0);
		AddInputBinding(VK_CONTROL, KeyEventType::key_up, OnCtrlUp, 0);

		AddInputBinding(VK_MENU, KeyEventType::key_down, OnAltDown, 0);
		AddInputBinding(VK_MENU, KeyEventType::key_up, OnAltUp, 0);
	}
}

namespace input
{
	coordinates<float> get_cursor_screen_location()
	{
		return cursor_location;
	}
	
	coordinates<float> get_cursor_map_location()
	{
		return ForgetteDirectX::screen_to_world(cursor_location);
	}
}