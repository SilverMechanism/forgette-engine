module;
module gm_survival;

import forgette;
import debug_unit;
import villager;
import spirit;
import player;
import core;
import timers;
import sprite;
import game_map;
import sector;
import directx;

void GM_Survival::spawn_wave()
{
	GameMap* game_map = get_engine()->get_map();
	
	TimerManager* timer_manager = TimerManager::Instance();
	timer_manager->CreateTimer(1.0f, [this, game_map](int calls)
	{ 
		ptr::watcher<Villager> new_villager;
		ptr::watcher<Spirit> new_spirit;
		
		get_engine()->spawn_entity<Villager>(coordinates<float>(0.0f, 0.0f).random_nearby(500.0f), new_villager);
		get_engine()->spawn_entity<Spirit>(new_spirit);
		
		new_spirit->inhabit(ptr::watcher<Unit>(new_villager));
	}, 20);
}

void GM_Survival::start()
{
	Forgette::Engine* engine = get_engine();
	Player* new_player = engine->create_player();
	
	ptr::watcher<DebugUnit> debug_unit;
	engine->spawn_entity<DebugUnit>(coordinates<float>(0, 0), debug_unit);
	
	new_player->possess_unit(ptr::watcher<Unit>(debug_unit));
	
	spawn_wave();
}