module;
module magic_text;

import directx;
import unit;
import health_element;
import collision_element;
import sprite;
import special_effect;
import timers;
import helpers;
import game_sound;

MagicText::MagicText()
{
	add_entity_identifier(EntityClass::MagicText);
	
	add_element<CollisionElement>();
	CollisionElement* ce = get_element<CollisionElement>();
	ce->on_collision = [this](Unit* other_unit) { on_collision(other_unit); };
	ce->collision_enabled = true;
	ce->radius = 8.0f;
	
	ce->collision_group = CollisionGroup::Projectile;
	ce->collides_with.insert(CollisionGroup::Unit);
	
	GameSound* new_discharge_sound = new GameSound("event:/Weapons/Discharge");
	discharge_sound = ptr::keeper<GameSound>(new_discharge_sound);
}

void MagicText::on_collision(Unit* other_unit)
{
	if (HealthElement* he = other_unit->get_element<HealthElement>())
	{
		he->damage(10.0f);
	}
	
	ptr::watcher<SpecialEffect> discharge_sfx;
	get_engine()->spawn_entity<SpecialEffect>(get_map_location(), discharge_sfx);
	
	TimerManager* timer_manager = TimerManager::Instance();
	
	timer_manager->CreateTimer(0.05f, [discharge_sfx](int calls) mutable
	{
		float fade = 1.0f - (0.2f * (calls));
		discharge_sfx->effect_color = {0.66f, 0.66f, 0.8f, fade};
		
		if (calls == 9)
		{
			discharge_sfx->pending_deletion = true;
		}
	}, 10);
	
	coordinates<float> location = get_map_location();
	discharge_sfx->effects[RenderGroup::PreGame] = {[discharge_sfx, location]()
	{
		ForgetteDirectX::draw_circle(discharge_sfx.get()->effect_color, location, 125.0f);
	}};
	
	std::vector<ptr::watcher<Entity>> struck_entities = Helpers::try_overlap(
		CollisionGroup::Unit,
		{CollisionGroup::Unit},
		get_map_location(),
		25.0f,
		{});
		
	for (auto& entity : struck_entities)
	{
		if (HealthElement* he = entity->get_element<HealthElement>())
		{
			he->damage(15.0f);
		}
	}
	
	discharge_sound->play(get_map_location());
	discharge_sound->event->setTimelinePosition(1);
	pending_deletion = true;
}

void MagicText::render_update(RenderGroup rg)
{
	FloatingText::render_update(rg);
	ForgetteDirectX::draw_unit_shadow(ForgetteDirectX::world_to_screen(get_map_location()), 18.0f);
}