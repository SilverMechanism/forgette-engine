module;
export module floating_text;

import unit;
import std;
import core;

export
{
	class FloatingText : public Unit
	{
	public:
		FloatingText();
		
		virtual void game_update(float delta_time) override;
		virtual void render_update(RenderGroup rg) override;
		
		coordinates<float> drift;
		std::string text;
		float life_time = 1.0f;
		float size = 16.0f;
		
		ColorParams color;
	};
}