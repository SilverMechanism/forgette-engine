module;
#include <windows.h>
#include <assert.h>
export module input;
import windows;
import std;

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

export namespace input
{
	std::map<int, InputBind> active_down_binds;
	
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
		assert(key < 254);
		
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

		AddInputBinding(VK_SHIFT, KeyEventType::key_down, OnShiftDown, 0);
		AddInputBinding(VK_SHIFT, KeyEventType::key_up, OnShiftUp, 0);

		AddInputBinding(VK_CONTROL, KeyEventType::key_down, OnCtrlDown, 0);
		AddInputBinding(VK_CONTROL, KeyEventType::key_up, OnCtrlUp, 0);

		AddInputBinding(VK_MENU, KeyEventType::key_down, OnAltDown, 0);
		AddInputBinding(VK_MENU, KeyEventType::key_up, OnAltUp, 0);
	}
}