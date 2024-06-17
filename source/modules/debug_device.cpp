module;
module debug_device;

import debug_action;
import input;
import helpers;
import directx;
import timers;
import sound;
import special_effect;
import forgette;
import health_element;

DebugDevice::DebugDevice()
{
	should_game_update = true;
	should_render_update = true;
	
	uses[Command::Primary] = [this](Unit* user, Target& target)
	{
		this->fire_text(user, target);
	};
	uses[Command::Secondary] = [this](Unit* user, Target& target)
	{
		this->discharge(user, target);
	};
	
	primary_sound = new GameSound("event:/Interactables/Wooden Collision");
	secondary_sound = new GameSound("event:/Weapons/Discharge");
}

DebugDevice::~DebugDevice()
{
	if (primary_sound)
	{
		delete primary_sound;
	}
	if (secondary_sound)
	{
		delete secondary_sound;
	}
}

void DebugDevice::game_update(float delta_time)
{
	
}

void DebugDevice::render_update(RenderGroup rg)
{
	
}

void DebugDevice::reload(Unit* user)
{
	Helpers::create_floating_text(
		"Reloading...", 
		16.0f, 
		user->get_map_location() + coordinates<float>(0.0f, 20.0f).view_isometric(), 
		reload_time, 
		coordinates<float>(0.0f, 8.0f).view_isometric(), 
		RenderGroup::Debug, 
		16.0f);
	
	reloading = true;
	TimerManager* timer_manager = TimerManager::Instance();
	timer_manager->CreateTimer(reload_time, [this](int calls)
	{
		clip_current = clip_maximum;
		reloading = false;
	}, 1);
}

void DebugDevice::fire_text(Unit* user, Target& target)
{
	if (clip_current <= 0)
	{
		if (!reloading)
		{
			reload(user);
		}
		
		return;
	}
	
	Helpers::create_magic_text(
		"Pew!", 
		16.0f,
		target.location,
		6.0f,
		user->get_map_location().towards(input::get_cursor_map_location()) * 233.0f,
		RenderGroup::Game,
		48.0f,
		{user->id});
		
	primary_sound->play();
		
	clip_current--;
}

void DebugDevice::discharge(Unit* user, Target& target)
{
	secondary_sound->play();
	TimerManager* timer_manager = TimerManager::Instance();
	
	coordinates<float> location = target.location;
	timer_manager->CreateTimer(0.15f, [this, user, location](int calls)
	{
		do_discharge(user, location);
	}, 1);
}

void DebugDevice::do_discharge(Unit* user, coordinates<float> location)
{
	ptr::watcher<SpecialEffect> discharge_sfx;
	get_engine()->spawn_entity<SpecialEffect>(user->get_map_location(), discharge_sfx);
	
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
	
	discharge_sfx->effects[RenderGroup::PreGame] = {[discharge_sfx, location]()
	{
		ForgetteDirectX::draw_circle(discharge_sfx.get()->effect_color, location, 256.0f);
	}};
	
	std::cout << "ID: " << user->id << std::endl;
	std::vector<ptr::watcher<Entity>> struck_entities = Helpers::try_overlap(
		CollisionGroup::Unit,
		{CollisionGroup::Unit},
		location,
		55.0f,
		{user->id});
		
	for (auto& entity : struck_entities)
	{
		if (HealthElement* he = entity->get_element<HealthElement>())
		{
			he->damage(200.0f);
		}
	}
}