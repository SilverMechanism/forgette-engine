module;
export module collision_element;

import element;
import unit;
import core;

export
{
	class CollisionElement : public Element
	{
	public:
		float radius = 8.0f;
		
		CollisionGroup collision_group { CollisionGroup::Unit };
		std::set<CollisionGroup> collides_with;
		
		bool collision_enabled = true;
		std::function<void(Unit*)> on_collision;
		
		std::vector<std::int64_t> ignored_entities;
	};
}