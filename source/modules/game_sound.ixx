module;
#include <fmod/fmod_studio.hpp>
export module game_sound;

import sound;
import std;

export
{
	class GameSound
	{
	public:
		GameSound();
		~GameSound();
		
		GameSound(std::string _event_path);
		
		std::string event_path;
		
		void play();
		void reload();
		
		FMOD::Studio::EventInstance* event = nullptr;
	};
}