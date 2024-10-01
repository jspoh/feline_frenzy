/*****************************************************************//**
 * \file   sysAudio.cpp
 * \brief  Audio manager function definitions 
 *
 * \author Bryan Lim
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysAudio.h"
#include "../headers/Core/Engine.h"
#include "../headers/Components/cAudio.h"

// Create Fmod instance
Audio::Manager::Manager()
{
	FMOD_RESULT result;
	result = FMOD::System_Create(&fmod_system);
	// Check if fmod studio system is created
	if (result != FMOD_OK)
	{
		cerr << "CREATION OF FMOD STUDIO SYSTEM FAILED!" << endl;
		exit(-1);
	}
	else {
		cout << "FMOD SYS CREATED!" << endl;
	}

	result = fmod_system->init(512, FMOD_INIT_NORMAL, 0);
	// Check if fmod studio system is initialized
	if (result != FMOD_OK)
	{
		cerr << "INITIALIZATION OF STUDIO FMOD SYSTEM FAILED!" << endl;
		exit(-1);
	}
	else {
		cout << "FMOD INITIALIZED!" << endl;
	}
}

void Audio::Manager::init()
{

}

void Audio::Manager::update()
{
	fmod_system->update();
	// For debug purposes
	// cout << "AUDIO SYSTEM IS UPDATING" << endl;
	for (const auto& entity : entities)
	{	
		// Check entity contain sound component
		if (NIKEEngine.checkEntityComponent<Audio::cAudio>(entity)) 
		{
			Audio::cAudio& c_audio = NIKEEngine.getEntityComponent<Audio::cAudio>(entity);
			if (!c_audio.is_played)
			{
				NEAudioPlay(c_audio.audio, c_audio.audio_group, c_audio.volume, c_audio.pitch, 0);
				c_audio.is_played = true;
			}
		}
	}

}

// Release Fmod system
Audio::Manager::~Manager()
{
	// Release fmod studio
	fmod_system->release();
}

Audio::NE_AUDIO Audio::Manager::NEAudioLoadSound(std::string const& file_path)
{
	FMOD::Sound* temp = nullptr;
	FMOD_RESULT result;
	// Create sound to be pushed into container
	result = fmod_system->createSound(file_path.c_str(), FMOD_DEFAULT, nullptr, &temp);

	// Check for audio file validadity
	if (result != FMOD_OK)
	{
		throw std::runtime_error("YOUR FILE WRONG BODO");
	}
	else {
		cout << "Audio loaded!" << endl;
	}

	return std::shared_ptr<FMOD::Sound>(temp, [](FMOD::Sound*){});
}

Audio::NE_AUDIO Audio::Manager::NEAudioLoadMusic(std::string const& file_path)
{

	FMOD::Sound* temp = nullptr;
	FMOD_RESULT result;
	// Create sound to be pushed into container
	// !!! FMOD will handle the reading from file using createStream function
	result = fmod_system->createStream(file_path.c_str(), FMOD_CREATESTREAM, nullptr, &temp);
	
	// Check for audio file validadity
	if (result != FMOD_OK)
	{
		throw std::runtime_error("YOUR FILE WRONG BODO");
	}

	return std::shared_ptr<FMOD::Sound>(temp, [](FMOD::Sound*) {});
}

Audio::NE_AUDIO_GROUP Audio::Manager::CreateAudioGroup(std::string const& audio_group_tag)
{

	NE_AUDIO_RESULT result{};
	FMOD::ChannelGroup* temp = nullptr;
	result = fmod_system->createChannelGroup(audio_group_tag.c_str(), &temp);
	if (result != FMOD_OK)
	{
		throw std::runtime_error("AUDIO GROUP NOT INITIALIZED");
	}
	else
	{
		cout << "AUDIO GROUP INITIALIZED" << endl;
	}
	return std::shared_ptr<FMOD::ChannelGroup>(temp, [](FMOD::ChannelGroup*){});
}

void Audio::Manager::NEAudioPlay(NE_AUDIO audio, NE_AUDIO_GROUP group, float vol, float pitch, bool loop)
{
	fmod_system->playSound(audio.get(), group.get(), 0, nullptr);
	cout << "AUDIO PLAY" << endl;

	group->setVolume(vol);
	group->setPitch(pitch);
	if (loop)
	{
		group->setMode(FMOD_LOOP_NORMAL);
	}
	else {
		group->setMode(FMOD_LOOP_OFF);
	} 
}

void Audio::Manager::NEAudioStopGroup(NE_AUDIO_GROUP group)
{
	// This is same as UNREFERENCED_PARAMETER
	cout << "AUDIO STOPPED" << endl;
	group->stop();
}

void Audio::Manager::NEAudioPauseGroup(NE_AUDIO_GROUP group)
{
	group->setPaused(true);
}

void Audio::Manager::NEAudioResumeGroup(NE_AUDIO_GROUP group)
{
	group->setPaused(false);
}

void Audio::Manager::NEAudioSetGroupPitch(NE_AUDIO_GROUP group, float pitch)
{
	group->setPitch(pitch);
}

void Audio::Manager::NEAudioSetGroupVolume(NE_AUDIO_GROUP group, float vol)
{
	group->setVolume(vol);
}

void Audio::Manager::NEAudioUnloadGroup(NE_AUDIO_GROUP group)
{
	group->release();
}

void Audio::Manager::NEAudioUnloadAudio(NE_AUDIO audio)
{
	audio->release();
}

void Audio::Manager::IsPlaying(NE_AUDIO audio)
{
	static_cast<void>(audio);
}
