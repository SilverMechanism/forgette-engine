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
		
		Action* action;
		
		void (*process_executor)(Unit*, Action*) = nullptr;
		
		template <typename T>
		void use(Unit* executor, T& target)
		{
			if (process_executor)
			{
				process_executor(executor, action);
			}
			action->try_execute(target);
		}
	};
}