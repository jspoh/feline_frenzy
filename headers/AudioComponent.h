#pragma once

#include "stdafx.h"

/************************************
* SOME NOTES TO TAKE NOTE OF: 
* - AUDIO WILL BE PLAYED LIKE AE
* - THERE WILL BE AUIDIO GROUP AND AUDIO
/****************************************/


using NE_AUDIO = FMOD::Sound*;
using NE_AUDIO_GROUP = FMOD::ChannelGroup*;

// Audio group will be tagged with a string tag
using AUDIO_GROUP = std::unordered_map<std::string, NE_AUDIO_GROUP>;

// Key will be the audio ID as a string
// Value will be audio
using AUDIO_MAP = std::unordered_map<std::string, NE_AUDIO>;

using NE_AUDIO_RESULT = FMOD_RESULT;


namespace Audio {

	struct AudioComponents
	{
		// Fmod stuff
		FMOD::System* fmod_system = nullptr;

		//// Fmod studio system
		//FMOD::Studio::System* fmod_studio_system;

		// File buffer to read .wav files and stuff
		char* file_buffer = nullptr;

		// Init the maps
		AUDIO_GROUP audio_group_map;
		AUDIO_MAP audio_map;

		// Volume 
		float volume;

		// Pitch
		float pitch;
		
	};

}