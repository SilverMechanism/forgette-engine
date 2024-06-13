module;
export module action;
import core;

export
{
	class Action
	{
	public:
		std::map<std::type_index, std::function<void(std::any)>> execute_functions;
		
		template <typename T>
		T get_target_value(const std::any& operand)
		{
			return std::any_cast<T>(operand);
		}
		
		template <typename T>
		void try_execute(T& target)
		{
			std::type_index target_type = std::type_index(typeid(target));
			auto iterator = execute_functions.find(target_type);
			if (iterator != execute_functions.end())
			{
				iterator->second(target);
			}
			else
			{
				std::cout << "Invalid target" << std::endl;
			}
		}
	};
}