module;
module gm_survival;

import forgette;
import debug_unit;
import villager;
import spirit;
import player;
import core;
import timers;

void GM_Survival::spawn_wave()
{
	TimerManager* timer_manager = TimerManager::Instance();
	timer_manager->CreateTimer(3.0f, [this]()
	{ 
		ptr::watcher<Villager> new_villager;
		ptr::watcher<Spirit> new_spirit;
		
		get_engine()->spawn_entity<Villager>(coordinates<float>(0.0f, 0.0f).random_nearby(200.0f), new_villager);
		get_engine()->spawn_entity<Spirit>(new_spirit);
		
		new_spirit->inhabit(ptr::watcher<Unit>(new_villager));
	}, 10);
}

void GM_Survival::start()
{
	Forgette::Engine* engine = get_engine();
	Player* new_player = engine->create_player();
	
	ptr::watcher<DebugUnit> debug_unit;
	engine->spawn_entity<DebugUnit>(coordinates<float>(0, 0), debug_unit);
	
	new_player->possess_unit(ptr::watcher<Unit>(debug_unit));
	
	ptr::watcher<Villager> new_villager;
	engine->spawn_entity<Villager>(coordinates(-100, -75), new_villager);
	
	ptr::watcher<Spirit> new_spirit;
	engine->spawn_entity<Spirit>({0,0}, new_spirit);
	
	new_spirit->inhabit(ptr::watcher<Unit>(new_villager));
	
	spawn_wave();
}