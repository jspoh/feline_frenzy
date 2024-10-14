/*****************************************************************//**
 * \file   sysAudio.h
 * \brief  Audio system manager function declarations
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef AUDIOSYS_HPP
#define AUDIOSYS_HPP

#include "../headers/Managers/mSystem.h"

/************************************
* SOME NOTES TO TAKE NOTE OF:
* - AUDIO WILL BE PLAYED LIKE AE
* - THERE WILL BE AUIDIO GROUP AND AUDIO
/****************************************/

namespace Audio {

	using NE_AUDIO = std::shared_ptr<FMOD::Sound>;
	using NE_AUDIO_GROUP = std::shared_ptr<FMOD::ChannelGroup>;

	// Audio group will be tagged with a string tag
	using AUDIO_GROUP = std::unordered_map<std::string, NE_AUDIO_GROUP>;

	// Key will be the audio ID as a string
	// Value will be audio
	using AUDIO_MAP = std::unordered_map<std::string, NE_AUDIO>;

	using NE_AUDIO_RESULT = FMOD_RESULT;

	class Manager : public System::ISystem
	{
	public:

		// Ctor - Create Fmod instance
		Manager();

		//Singleton Of Manager Class
		static std::shared_ptr<Manager> getInstance() {
			static std::shared_ptr<Manager> instance{ std::make_shared<Manager>() };
			return instance;
		}

		// Init Audio system
		void init() override;

		// Update Audio system
		void update() override;

		std::string getSysName() override
		{
			return "Audio System";
		}

		//Default Destructor
		~Manager() override;

		// Load audio file (sound)
		NE_AUDIO NEAudioLoadSound(std::string const& file_path);

		// Load audio file (music)
		NE_AUDIO NEAudioLoadMusic(std::string const& file_path);

		// Create audio group
		NE_AUDIO_GROUP CreateAudioGroup(std::string const& audio_group_tag);

		// Play music
		void NEAudioPlay(NE_AUDIO audio, NE_AUDIO_GROUP, float vol, float pitch, bool );

		// Stop sound
		void NEAudioStopGroup(NE_AUDIO_GROUP group);

		// Play sound
		void NEAudioPauseGroup(NE_AUDIO_GROUP group);

		// Resume music
		void NEAudioResumeGroup(NE_AUDIO_GROUP group);

		// Set pitch
		void NEAudioSetGroupPitch(NE_AUDIO_GROUP group, float pitch);

		// Set volume
		void NEAudioSetGroupVolume(NE_AUDIO_GROUP group, float vol);

		// Unload audio group
		void NEAudioUnloadGroup(NE_AUDIO_GROUP group);

		// Unload audio
		void NEAudioUnloadAudio(NE_AUDIO audio);

		// Check if audio playimg
		void IsPlaying(NE_AUDIO audio);

	private:
		// Delete Copy Constructor & Copy Assignment
		Manager(Manager const& rhs) = delete;
		void operator=(Manager const& copy) = delete;
		
		// Fmod stuff
		FMOD::System* fmod_system = nullptr;
	};

}

#endif