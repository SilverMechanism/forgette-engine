module;
module game_sound;

import sound;

GameSound::GameSound()
{

}

GameSound::GameSound(std::string _event_path)
{
	event_path = _event_path;
	reload();
}

GameSound::~GameSound()
{
	if (event)
	{
		event->release();
	}
}

void GameSound::reload()
{
	if (event)
	{
		event->release();
	}
	
	event = Sound::load_event(event_path);
}

void GameSound::play()
{
	if (!event)
	{
		reload();
	}
	
	event->start();
}