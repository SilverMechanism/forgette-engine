module;
export module helpers;

import std;
import core;

export namespace Helpers
{
	void create_floating_text(std::string text, float size, coordinates<float> map_location, float life_time, coordinates<float> input = {0.0f, 0.0f}, RenderGroup render_group = RenderGroup::Debug, float z = 0.0f);
	void create_magic_text(std::string text, float size, coordinates<float> map_location, float life_time, coordinates<float> input = {0.0f, 0.0f}, RenderGroup render_group = RenderGroup::Debug, float z = 0.0f, std::vector<std::int64_t> ignored_entities = {});
}