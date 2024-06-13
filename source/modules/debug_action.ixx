module;
export module debug_action;

import core;
import action;
import floating_text;

export
{
	class DebugAction : public Action
	{
	public:
		coordinates<float> text_drift;
		
		void execute_coords(std::any target)
		{
			coordinates<float> coords = get_target_value<coordinates<float>>(target);
			create_floating_text("Pew!", 16.0f, coords, 6.0f, text_drift, RenderGroup::Game, 48.0f);
		}
		
		DebugAction()
		{
			execute_functions[std::type_index(typeid(coordinates<float>))] = [this](std::any target) {
                this->execute_coords(target);
            };
		}
	};
}