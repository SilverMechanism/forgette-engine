module;
export module input_handler;
import std;
import input;
import core;

export
{
	struct InputBinding
	{
		std::string name;
		input::KeyEventType key_event = input::KeyEventType::key_down;
		std::uint8_t priority = 0;
	};
	
	class InputHandler
	{
	public:
		virtual ~InputHandler() = default;
		
		virtual void bind_inputs(std::vector<InputBinding> inputs) = 0;
	};
}