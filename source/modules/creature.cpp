module;
module creature;

import collision_element;
import health_element;

Creature::Creature()
{
	add_element<CollisionElement>();
	add_element<HealthElement>();
}