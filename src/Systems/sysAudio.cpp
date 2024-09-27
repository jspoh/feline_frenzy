/*****************************************************************//**
 * \file   sysAudio.cpp
 * \brief  Audio manager function definitions 
 *
 * \author Bryan Lim
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysAudio.h"
#include "../headers/Systems/sysInput.h"

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

	result = fmod_system->init(512, FMOD_STUDIO_INIT_NORMAL, 0);
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
}

// Release Fmod system
Audio::Manager::~Manager()
{
	// Release fmod studio
	fmod_system->release();
}

Audio::NE_AUDIO Audio::Manager::NEAudioLoadSound(std::string const& file_path)
{
	NE_AUDIO temp = nullptr;
	FMOD_RESULT result;
	// Create sound to be pushed into container
	result = fmod_system->createSound(file_path.c_str(), FMOD_DEFAULT, nullptr, &temp);

	// Check for audio file validadity
	if (result != FMOD_OK)
	{
		cerr << "YOUR FILE WRONG BODO" << endl;
		return nullptr;
	}

	return temp;
}

Audio::NE_AUDIO Audio::Manager::NEAudioLoadMusic(std::string const& file_path)
{

	NE_AUDIO temp;
	FMOD_RESULT result;
	// Create sound to be pushed into container
	// !!! FMOD will handle the reading from file using createStream function
	result = fmod_system->createStream(file_path.c_str(), FMOD_CREATESTREAM, nullptr, &temp);
	
	// Check for audio file validadity
	if (result != FMOD_OK)
	{
		cerr << "YOUR FILE WRONG BODO" << endl;
		return temp;
	}

	return temp;
}

Audio::NE_AUDIO_GROUP Audio::Manager::CreateAudioGroup(std::string const& audio_group_tag)
{

	NE_AUDIO_RESULT result{};
	NE_AUDIO_GROUP temp = nullptr;
	result = fmod_system->createChannelGroup(audio_group_tag.c_str(), &temp);
	if (result != FMOD_OK)
	{
		cerr << "AUDIO GROUP NOT INITIALIZED" << endl;
		return temp;
	}
	else
	{
		cout << "AUDIO GROUP INITIALIZED" << endl;
	}
	return temp;
}

void Audio::Manager::NEAudioPlay(NE_AUDIO audio, NE_AUDIO_GROUP group, float vol, float pitch, bool loop)
{
	fmod_system->playSound(audio, group, 0, nullptr);	
	// This is same as UNREFERENCED_PARAMETER
	static_cast<void>(vol);
	static_cast<void>(pitch);
	static_cast<void>(loop);
}

void Audio::Manager::NEAudioStopGroup(std::string const& tag)
{
	// This is same as UNREFERENCED_PARAMETER
	static_cast<void>(tag);
	//if (Input::Manager::getInstance().key_is_pressed(GLFW_KEY_Q))
	//{
	//	audio_group_map[tag]->stop();
	//}
}

void Audio::Manager::NEAudioPauseGroup(NE_AUDIO_GROUP group)
{
	static_cast<void>(group);

}

void Audio::Manager::NEAudioResumeGroup(NE_AUDIO_GROUP group)
{
	static_cast<void>(group);
}

void Audio::Manager::NEAudioSetGroupPitch(NE_AUDIO_GROUP group, float pitch)
{
	static_cast<void>(group);
	static_cast<void>(pitch);
}

void Audio::Manager::NEAudioSetGroupVolume(NE_AUDIO_GROUP group)
{
	static_cast<void>(group);
}

void Audio::Manager::NEAudioUnloadGroup(NE_AUDIO_GROUP group)
{
	static_cast<void>(group);
}

void Audio::Manager::NEAudioUnloadAudio(NE_AUDIO audio)
{
	static_cast<void>(audio);
}

void Audio::Manager::IsPlaying(NE_AUDIO audio)
{
	static_cast<void>(audio);
}
