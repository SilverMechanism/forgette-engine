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
import movement_element;

DebugDevice::DebugDevice()
{
	should_game_update = true;
	should_render_update = true;
	
	uses[Command::Primary] = [this](Unit* user, Target& target)
	{
		this->fire_text(user, target);
	};
	uses[Command::Tertiary] = [this](Unit* user, Target& target)
	{
		this->charge(user);
	};
	uses[Command::StopTertiary] = [this](Unit* user, Target& target)
	{
		this->stop_charge();
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
	if (!charging)
	{
		if (!overload_on_cooldown && overload_power > (overload_power_maximum/4.0f))
		{
			do_overload();
		}
		else if (overload_power > 0.0f)
		{
			overload_power -= (overload_charge_rate * 2.0f * delta_time);
		}
	}
	else if (!overload_on_cooldown)
	{
		overload_power += (overload_charge_rate * delta_time);
		if (overload_power >= overload_power_maximum)
		{
			do_overload();
		}
	}
}

void DebugDevice::render_update(RenderGroup rg)
{
	if (draw_ui)
	{
		if (rg == RenderGroup::UI)
		{
			// draw hp
			coordinates<float> resolution = ForgetteDirectX::get_resolution();
			coordinates<float> draw_position = {resolution.x*0.94f, resolution.y*0.925f};
			
			std::string ammo_text = std::format("{} / {}", clip_current, clip_maximum);
			
			ColorParams text_color = ColorParams(0.8f, 0.8f, 0.8f, 1.0f);
			
			ForgetteDirectX::draw_text(ammo_text, draw_position, 72.0f, true, true, text_color);
		}
	}
}

void DebugDevice::reload(const Unit* user)
{
	if (reloading || clip_current == clip_maximum)
	{
		return;
	}
	
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

void DebugDevice::charge(Unit* user)
{
	if (clip_current > 0)
	{
		charging = true;
	}
	else
	{
		reload(user);
	}
}

void DebugDevice::stop_charge()
{
	charging = false;
}

void DebugDevice::do_overload()
{
	clip_current--;
	
	float charged_power = overload_power;
	overload_power = 0.0f;
	
	if (!owner)
	{
		std::cout << "No owner" << std::endl;
		return;
	}
	
	ptr::watcher<SpecialEffect> discharge_sfx;
	get_engine()->spawn_entity<SpecialEffect>(owner->get_map_location(), discharge_sfx);
	
	TimerManager* timer_manager = TimerManager::Instance();
	
	float fade = 1.0f;
	timer_manager->CreateTimer(0.05f, [discharge_sfx, fade](int calls) mutable
	{
		fade = 1.0f - (0.2f * (calls));
		discharge_sfx->effect_color = {0.66f, 0.66f, 0.8f, fade};
		if (calls == 9)
		{
			discharge_sfx->pending_deletion = true;
		}
	}, 10);
	
	float charge_percent = (charged_power / overload_power_maximum);
	float scaled_aoe = overload_aoe * charge_percent;
	coordinates<float> location = owner->get_map_location();
	float circle_size = scaled_aoe * 5.0f;
	discharge_sfx->effects[RenderGroup::PreGame] = {[discharge_sfx, location, circle_size]()
	{
		ForgetteDirectX::draw_circle(discharge_sfx.get()->effect_color, location, circle_size);
	}};
	
	// std::cout << "ID: " << owner->id << std::endl;
	std::vector<ptr::watcher<Entity>> struck_entities = Helpers::try_overlap(
		CollisionGroup::Unit,
		{CollisionGroup::Unit},
		location,
		scaled_aoe,
		{});
		
	for (auto& entity : struck_entities)
	{
		if (HealthElement* he = entity->get_element<HealthElement>())
		{
			he->damage(overload_damage * charge_percent);
		}
	}
	
	overload_on_cooldown = true;
	timer_manager->CreateTimer(overload_cooldown, [this](int calls) mutable
	{
		overload_on_cooldown = false;
	});
	
	MovementElement* me = owner->get_element<MovementElement>();
	me->velocity = me->velocity + (me->velocity.normalize() * 500.0f);
	owner->z = 256.0f;
	secondary_sound->play();
}