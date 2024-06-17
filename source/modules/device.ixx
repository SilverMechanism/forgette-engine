module;
#include "defines.h"
export module device;

import entity;
import std;
import action;
import unit;
import core;

export
{
	class Device : public Entity
	{
	protected:
		
	public:
		Device();
		
		void use(Command command, Unit* user, Target& target);
		
		static void report_target_invalid(Unit* user, Target& target);
		
	protected:
		std::map<Command, std::function<void(Unit* user, Target& target)>> uses;
	};
}