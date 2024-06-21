module;
module game_sound;

import sound;
import directx;
import core;

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

void GameSound::play(coordinates<float> map_location)
{
	coordinates<float> screen_location = ForgetteDirectX::world_to_screen(map_location);
    FMOD_3D_ATTRIBUTES attributes = { 0 };
    attributes.position = { screen_location.x/100, screen_location.y/100, 0.0f };
    attributes.forward = { 0.0f, 0.0f, 1.0f };
    attributes.up = { 0.0f, -1.0f, 0.0f };

    event->set3DAttributes(&attributes);
    
	if (!event)
	{
		reload();
	}
	
	event->start();
}

void GameSound::play()
{
	if (!event)
	{
		reload();
	}
	
	FMOD_3D_ATTRIBUTES attributes = { 0 };
	Sound::studioSystem->getListenerAttributes(0, &attributes);
	event->set3DAttributes(&attributes);
	event->start();
}