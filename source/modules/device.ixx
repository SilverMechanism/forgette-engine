module;
export module device;

import entity;
import std;
import action;

class Unit;

template <typename T>
concept ActionCallable = requires(Action* action, T& target)
{
	{ action->execute(target) };
};

export
{	
	class Device : public Entity
	{
	public:
		Device();
		
		Action* action;
		
		template <typename T>
		void use(Unit& executor, T& target)
		{
			if constexpr (ActionCallable<T>)
			{
				action->execute(target);
			}
			else
			{
				// Lets handle this with a cute little in-game message when it happens
				std::cout << "Invalid target!" << std::endl;
			}
		}
	};
}