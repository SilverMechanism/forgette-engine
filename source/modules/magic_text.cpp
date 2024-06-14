module;
module magic_text;

import directx;
import unit;

MagicText::MagicText()
{
	collision_enabled = true;
	radius = 8.0f;
	
	collision_group = CollisionGroup::Projectile;
	collides_with.insert(CollisionGroup::Unit);
}

void MagicText::on_collision(Unit* other_unit)
{
	pending_deletion = true;
	other_unit->damage(10.0f);
}

void MagicText::render_update()
{
	FloatingText::render_update();
	ForgetteDirectX::draw_unit_shadow(ForgetteDirectX::world_to_screen(get_map_location()), 18.0f);
}