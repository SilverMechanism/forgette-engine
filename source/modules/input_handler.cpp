module;
export module input_handler;

import std;
import input;
import core;

export
{
	class InputHandler
	{
	public:
		virtual ~InputHandler() = default;
		
		virtual void bind_inputs() = 0;
	};
}