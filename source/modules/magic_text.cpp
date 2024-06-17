module;
module magic_text;

import directx;
import unit;
import health_element;
import collision_element;

MagicText::MagicText()
{
	add_element<CollisionElement>();
	CollisionElement* ce = get_element<CollisionElement>();
	ce->on_collision = [this](Unit* other_unit) { on_collision(other_unit); };
	ce->collision_enabled = true;
	ce->radius = 8.0f;
	
	ce->collision_group = CollisionGroup::Projectile;
	ce->collides_with.insert(CollisionGroup::Unit);
}

void MagicText::on_collision(Unit* other_unit)
{
	pending_deletion = true;
	
	
	if (HealthElement* he = other_unit->get_element<HealthElement>())
	{
		he->damage(10.0f);
	}
}

void MagicText::render_update(RenderGroup rg)
{
	FloatingText::render_update(rg);
	ForgetteDirectX::draw_unit_shadow(ForgetteDirectX::world_to_screen(get_map_location()), 18.0f);
}