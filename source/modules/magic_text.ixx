module;
export module magic_text;

import floating_text;
import forgette;

class GameSound;

export class Unit;

export
{
	class MagicText : public FloatingText
	{
	public:
		MagicText();
		void on_collision(Unit* other_unit);
		virtual void render_update(RenderGroup rg) override;
		
		ptr::keeper<GameSound> discharge_sound;
	};
}