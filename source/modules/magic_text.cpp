module;
module magic_text;

import unit;

MagicText::MagicText()
{
	collision_enabled = true;
	radius = 4.0f;
	
}

void MagicText::on_collision(Unit* other_unit)
{
	pending_deletion = true;
	other_unit->damage(10.0f);
}