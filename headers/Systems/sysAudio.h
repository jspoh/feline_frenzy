#pragma once

#ifndef AUDIOSYS_HPP
#define AUDIOSYS_HPP

#include "../headers/Core/stdafx.h"
#include "../headers/Core/Engine.h"
#include "../headers/Components/cAudio.h"
#include "../headers/Systems/sysInput.h"

/************************************
* SOME NOTES TO TAKE NOTE OF:
* - AUDIO WILL BE PLAYED LIKE AE
* - THERE WILL BE AUIDIO GROUP AND AUDIO
/****************************************/

namespace AudioSystem {

	class AudioSystem : public System::ISystem
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
		NE_AUDIO NEAudioLoadSound(std::string const& file_path, std::string const& tag);

		// Load audio file (music)
		NE_AUDIO NEAudioLoadMusic(std::string const& file_path, std::string const& tag);

		// Create audio group
		NE_AUDIO_GROUP CreateAudioGroup(std::string const& audio_group_tag);

		// Gettors for audio group and audio
		NE_AUDIO_GROUP GetAudioGroup(std::string const& tag);

		NE_AUDIO GetAudio(std::string const& tag);

		// Check audio group exist
		bool NEAudioIsValidGroup(std::string const& audio_group_tag);

		// Play music
		void NEAudioPlay(NE_AUDIO audio, NE_AUDIO_GROUP, float vol, float pitch, bool loop);

		void NEAudioPlay(std::string const&, std::string const&, float vol, float pitch, bool loop);

		// Stop sound
		void NEAudioStopGroup(std::string const& tag);

		// Check audio exist
		bool NEAudioIsValid(NE_AUDIO_GROUP group);

		// Play sound
		void NEAudioPauseGroup(NE_AUDIO_GROUP group);

		// Resume music
		void NEAudioResumeGroup(NE_AUDIO_GROUP group);

		// Set pitch
		void NEAudioSetGroupPitch(NE_AUDIO_GROUP group, float pitch);

		// Set volume
		void NEAudioSetGroupVolume(NE_AUDIO_GROUP group);

		// Unload audio group
		void NEAudioUnloadGroup(NE_AUDIO_GROUP group);

		// Unload audio
		void NEAudioUnloadAudio(NE_AUDIO audio);

		// Check if audio playimg
		void IsPlaying(NE_AUDIO audio);

	private:
		// Delete Copy Constructor & Copy Assignment
		AudioSystem(AudioSystem const& rhs) = delete;
		void operator=(AudioSystem const& copy) = delete;
		
		// Audio Components (here for the time being)
		// std::unique_ptr<Audio::AudioComponents> audio_components = std::make_unique<Audio::AudioComponents>();
		// Fmod stuff
		FMOD::System* fmod_system = nullptr;

		// Init the maps
		AUDIO_GROUP audio_group_map;
		AUDIO_MAP audio_map;
	};

}

#endif