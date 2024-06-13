module;
module helpers;

import core;
import std;
import floating_text;
import magic_text;
import forgette;

void Helpers::create_magic_text(std::string text, float size, coordinates<float> map_location, float life_time, coordinates<float> input, RenderGroup render_group, float z, std::vector<std::int64_t> ignored_entities)
{
	ptr::watcher<MagicText> floating_text;
	get_engine()->spawn_entity<MagicText>(map_location, floating_text);
	
	if (MagicText* ft_ptr = floating_text.get())
	{
		floating_text->text = text;
		floating_text->life_time = life_time;
		floating_text->size = size;
		floating_text->render_group = render_group;
		floating_text->drift = input;
		floating_text->z = z;
		floating_text->ignored_entities = ignored_entities;
	}
}

void Helpers::create_floating_text(std::string text, float size, coordinates<float> map_location, float life_time, coordinates<float> input, RenderGroup render_group, float z)
{
	ptr::watcher<FloatingText> floating_text;
	get_engine()->spawn_entity<FloatingText>(map_location, floating_text);
	
	if (FloatingText* ft_ptr = floating_text.get())
	{
		floating_text->text = text;
		floating_text->life_time = life_time;
		floating_text->size = size;
		floating_text->render_group = render_group;
		floating_text->drift = input;
		floating_text->z = z;
	}
}