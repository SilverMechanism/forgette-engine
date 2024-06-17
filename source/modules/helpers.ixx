module;
#include "defines.h"
export module helpers;

import std;
import core;

export class Entity;

export namespace Helpers
{
	void create_floating_text(std::string text, float size, coordinates<float> map_location, float life_time, coordinates<float> input = {0.0f, 0.0f}, RenderGroup render_group = RenderGroup::Debug, float z = 0.0f);
	void create_magic_text(std::string text, float size, coordinates<float> map_location, float life_time, coordinates<float> input = {0.0f, 0.0f}, RenderGroup render_group = RenderGroup::Debug, float z = 0.0f, std::vector<std::int64_t> ignored_entities = {});

	std::vector<ptr::watcher<Entity>> try_overlap(CollisionGroup collision_group, std::set<CollisionGroup> collides_with, coordinates<float> location, float radius, std::vector<int64> to_ignore);
}