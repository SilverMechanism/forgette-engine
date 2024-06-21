module;
#include <fmod/fmod_studio.hpp>
export module game_sound;

import sound;
import std;
import core;

export
{
	class GameSound
	{
	public:
		GameSound();
		~GameSound();
		
		GameSound(std::string _event_path);
		
		std::string event_path;
		
		void play(coordinates<float> map_location);
		void play();
		void reload();
		
		FMOD::Studio::EventInstance* event = nullptr;
	};
}