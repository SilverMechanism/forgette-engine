module;
export module debug_action;

import core;
import action;
import helpers;

export
{
	class DebugAction : public Action
	{
	public:
		coordinates<float> text_drift;
		std::vector<std::int64_t> entities_to_ignore;
		
		void execute_coords(std::any target)
		{
			coordinates<float> coords = get_target_value<coordinates<float>>(target);
			Helpers::create_magic_text("Pew!", 16.0f, coords, 6.0f, text_drift, RenderGroup::Game, 48.0f, entities_to_ignore);
		}
		
		DebugAction()
		{
			execute_functions[std::type_index(typeid(coordinates<float>))] = [this](std::any target) {
                this->execute_coords(target);
            };
		}
	};
}