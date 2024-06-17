module;
#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include <fmod/fmod_studio.hpp>
#include <windows.h>
module sound;

import std;
import directx;

namespace Sound
{
	bool initialize()
	{
		FMOD_RESULT result;
		
		// Initialize COM (important for certain audio drivers)
		HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
		{
		    std::cerr << "COM initialization failed! Error: " << hr << std::endl;
		    return false;
		}
		
		// Create the FMOD Studio system
		result = FMOD::Studio::System::create(&studioSystem);
		if (result != FMOD_OK)
		{
		    std::cerr << "FMOD Studio error! " << FMOD_ErrorString(result) << std::endl;
		    return false;
		}
		
		// Initialize the FMOD Studio system
		result = studioSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
		if (result != FMOD_OK)
		{
		    std::cerr << "FMOD Studio error! " << FMOD_ErrorString(result) << std::endl;
		    return false;
		}
		
		// Get the low-level FMOD Core system
		result = studioSystem->getCoreSystem(&coreSystem);
		if (result != FMOD_OK)
		{
		    std::cerr << "FMOD Core error! " << FMOD_ErrorString(result) << std::endl;
		    return false;
		}
		
		// Set output to default Windows device (optional, usually default)
		result = coreSystem->setDriver(0);  // 0 usually corresponds to the default device
		if (result != FMOD_OK)
		{
		    std::cerr << "FMOD Core error setting driver! " << FMOD_ErrorString(result) << std::endl;
		    return false;
		}

		FMOD::Studio::Bank* masterBank = nullptr;
		FMOD::Studio::Bank* stringsBank = nullptr;
		FMOD::Studio::Bank* editorBank = nullptr;
		
		std::string exe_dir = wstring_to_string(get_exe_dir() + L"sounds/");
		
		// Load the master bank file
		result = studioSystem->loadBankFile((exe_dir + "Master.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);
		if (result != FMOD_OK)
		{
		    std::cerr << "FMOD Studio error loading Master.bank! " << FMOD_ErrorString(result) << std::endl;
		    return false;
		}
		
		result = studioSystem->loadBankFile((exe_dir + "Editor.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &editorBank);
		if (result != FMOD_OK)
		{
		    std::cerr << "FMOD Studio error loading Editor.bank! " << FMOD_ErrorString(result) << std::endl;
		    return false;
		}
		
		result = studioSystem->loadBankFile((exe_dir + "SFX.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &editorBank);
		if (result != FMOD_OK)
		{
		    std::cerr << "FMOD Studio error loading SFX.bank! " << FMOD_ErrorString(result) << std::endl;
		    return false;
		}
		
		// Load the strings bank file
		result = studioSystem->loadBankFile((exe_dir + "Master.strings.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);
		if (result != FMOD_OK)
		{
		    std::cerr << "FMOD Studio error loading Master.strings.bank! " << FMOD_ErrorString(result) << std::endl;
		    return false;
		}

		return true;
	}
	
	FMOD::Studio::EventInstance* load_event(std::string event_path)
	{
		FMOD::Studio::EventDescription* event_description = nullptr;
	    FMOD::Studio::EventInstance* event_instance = nullptr;
	    
		FMOD_RESULT result = studioSystem->getEvent(event_path.c_str(), &event_description);
	    if (result != FMOD_OK)
	    {
	        std::cerr << "FMOD Studio error getting event description! " << FMOD_ErrorString(result) << std::endl;
	        return nullptr;
	    }
	
	    // Create an instance of the event
	    result = event_description->createInstance(&event_instance);
	    if (result != FMOD_OK)
	    {
	        std::cerr << "FMOD Studio error creating event instance! " << FMOD_ErrorString(result) << std::endl;
	        return nullptr;
	    }
	    
	    return event_instance;
	}
}