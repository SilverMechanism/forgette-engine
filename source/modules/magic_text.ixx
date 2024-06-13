module;
export module magic_text;

import floating_text;
import forgette;

export class Unit;

export
{
	class MagicText : public FloatingText
	{
	public:
		MagicText();
		virtual void on_collision(Unit* other_unit) override;
	};
}