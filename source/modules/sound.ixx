module;
#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include <fmod/fmod_studio.hpp>
export module sound;

import std;
import core;

export namespace Sound
{
	FMOD::Studio::System* studioSystem = nullptr;
	FMOD::System* coreSystem = nullptr;
	FMOD::Studio::EventInstance* eventInstance = nullptr;
	FMOD::Sound* sound = nullptr;
	FMOD::Channel* channel = nullptr;
	
	bool initialize();
	
	FMOD::Studio::EventInstance* load_event(std::string event_path);
}