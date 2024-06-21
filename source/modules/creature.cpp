module;
module creature;

import collision_element;
import health_element;
import movement_element;
import sprite;

Creature::Creature()
{
	add_element<CollisionElement>();
	add_element<HealthElement>();
	add_element<MovementElement>();
}