module;
export module element;

import core;

export class Entity;

export
{
	class Element
	{
	public:
		void set_owner(Entity* unit)
		{
			owner = unit;
		}
		Entity* get_owner()
		{
			return owner;
		}
		
	protected:
		Entity* owner = nullptr;
	};
}