#pragma once

#include "stdafx.h"

/************************************
* SOME NOTES TO TAKE NOTE OF: 
* - AUDIO WILL BE PLAYED LIKE AE
* - THERE WILL BE AUIDIO GROUP AND AUDIO
/****************************************/


using NE_AUDIO = FMOD::Sound*;


using NE_AUDIO_GROUP = FMOD::ChannelGroup*;


namespace Audio {

	struct AudioComponents
	{
		// Fmod stuff
		FMOD::System* fmod_system = nullptr;

		//// Fmod studio system
		//FMOD::Studio::System* fmod_studio_system;

		// Typedefs maps
		// Key will be the audio group
		// Value will be audio
		typedef std::unordered_map<NE_AUDIO_GROUP, NE_AUDIO> AUDIO_MAP;

		// File buffer to read .wav files and stuff
		char* file_buffer = nullptr;

		// Init the maps
		AUDIO_MAP audio_map;
		
	};

}