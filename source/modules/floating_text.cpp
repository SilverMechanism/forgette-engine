module;
export module floating_text;
import directx;
import std;
import core;
import forgette;

export
{
	class FloatingText
	{
	public:
		static void create(std::string text, float size, coordinates<float> map_location, float life_time);
	};
}

void FloatingText::create(std::string text, float size, coordinates<float> map_location, float life_time)
{
	map_location = ForgetteDirectX::world_to_screen(map_location);
	
	get_engine()->render_functions[0].emplace_back([text, map_location, size, life_time](float delta_time)
		{ 
			static float time_left = life_time;
			ForgetteDirectX::draw_text(text, map_location, size);
			time_left = time_left - delta_time;
			return time_left > 0;
		});
}