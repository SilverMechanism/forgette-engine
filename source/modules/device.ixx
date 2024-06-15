module;
export module device;

import entity;
import std;
import action;
import unit;

export
{	
	class Device : public Entity
	{
	public:
		Device();
		
		template<typename T>
		void use(Unit* user, T& target);
	};
}