#pragma once

#ifndef C_AUDIO_HPP
#define C_AUDIO_HPP


#include "../headers/Core/stdafx.h"

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
		//// Fmod studio system
		//FMOD::Studio::System* fmod_studio_system;

		// File buffer to read .wav files and stuff
		char* file_buffer = nullptr;

		// Volume 
		float volume{};

		// Pitch
		float pitch{};
		
	};

}

#endif