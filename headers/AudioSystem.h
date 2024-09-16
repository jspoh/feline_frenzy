#pragma once

#ifndef AUDIOSYS_HPP
#define AUDIOSYS_HPP

#include "stdafx.h"
#include "Engine.h"
#include "AudioComponent.h"

/************************************
* SOME NOTES TO TAKE NOTE OF:
* - AUDIO WILL BE PLAYED LIKE AE
* - THERE WILL BE AUIDIO GROUP AND AUDIO
/****************************************/

namespace AudioSystem {

	class AudioSystem : public System::Base
	{
	public:

		// Ctor - Create Fmod instance
		AudioSystem();

		// Singleton Of Audio System Class
		static AudioSystem& getInstance() {
			static AudioSystem instance;
			return instance;
		}

		// Init Audio system
		void init() override;

		// Update Audio system
		void update() override;

		//Default Destructor
		~AudioSystem() override;

		// Load audio file (sound)
		NE_AUDIO NEAudioLoadSound(std::string& file_path, NE_AUDIO_GROUP audio_group);

		// Load audio file (music)
		NE_AUDIO NEAudioLoadMusic(std::string& file_path, NE_AUDIO_GROUP audio_group);

	private:
		// Delete Copy Constructor & Copy Assignment
		AudioSystem(AudioSystem const& rhs) = delete;
		void operator=(AudioSystem const& copy) = delete;
		
		// Audio Components (here for the time being)
		std::unique_ptr<Audio::AudioComponents> audio_components = std::make_unique<Audio::AudioComponents>();
	};

}

#endif