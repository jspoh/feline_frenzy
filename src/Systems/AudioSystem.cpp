
#include "stdafx.h"
#include "AudioSystem.h"

// Create Fmod instance
AudioSystem::AudioSystem::AudioSystem()
{
	FMOD_RESULT result;
	result = FMOD::System_Create(&audio_components->fmod_system);
	// Check if fmod studio system is created
	if (result != FMOD_OK)
	{
		cerr << "CREATION OF FMOD STUDIO SYSTEM FAILED!" << endl;
		exit(-1);
	}
	else {
		cout << "FMOD SYS CREATED!" << endl;
	}

	result = audio_components->fmod_system->init(512, FMOD_STUDIO_INIT_NORMAL, 0);
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

void AudioSystem::AudioSystem::init()
{

}

void AudioSystem::AudioSystem::update()
{
	audio_components->fmod_system->update();
	// For debug purposes
	// cout << "AUDIO SYSTEM IS UPDATING" << endl;
}

// Release Fmod system
AudioSystem::AudioSystem::~AudioSystem()
{
	// Release fmod studio
	audio_components->fmod_system->release();
}

NE_AUDIO AudioSystem::AudioSystem::NEAudioLoadSound(std::string const& file_path, std::string const& tag)
{
	NE_AUDIO temp = nullptr;
	FMOD_RESULT result;
	// Create sound to be pushed into container
	result = audio_components->fmod_system->createSound(file_path.c_str(), FMOD_DEFAULT, nullptr, &temp);

	// Check for audio file validadity
	if (result != FMOD_OK)
	{
		cerr << "YOUR FILE WRONG BODO" << endl;
		return nullptr;
	}

	// Push sound after reading from file
	audio_components->audio_map[tag] = temp;

	return temp;
}

NE_AUDIO AudioSystem::AudioSystem::NEAudioLoadMusic(std::string const& file_path, std::string const& tag)
{

	NE_AUDIO temp;
	FMOD_RESULT result;
	// Create sound to be pushed into container
	// !!! FMOD will handle the reading from file using createStream function
	result = audio_components->fmod_system->createStream(file_path.c_str(), FMOD_CREATESTREAM, nullptr, &temp);
	
	// Check for audio file validadity
	if (result != FMOD_OK)
	{
		cerr << "YOUR FILE WRONG BODO" << endl;
		return temp;
	}

	// Push sound after reading from file
	audio_components->audio_map[tag] = temp;

	return temp;
}

NE_AUDIO_GROUP AudioSystem::AudioSystem::CreateAudioGroup(std::string const& audio_group_tag)
{
	// Check if the group already exists in the map
	if (audio_components->audio_group_map.find(audio_group_tag) != audio_components->audio_group_map.end())
	{
		cerr << "AUDIO GROUP ALREADY EXISTS" << endl;
	}

	NE_AUDIO_RESULT result{};
	NE_AUDIO_GROUP temp = nullptr;
	result = audio_components->fmod_system->createChannelGroup(audio_group_tag.c_str(), &temp);
	if (result != FMOD_OK)
	{
		cerr << "AUDIO GROUP NOT INITIALIZED" << endl;
		return temp;
	}
	else
	{
		cout << "AUDIO GROUP INITIALIZED" << endl;
	}
	// Push into audio group map
	audio_components->audio_group_map[audio_group_tag] = temp;
	return temp;
}

bool AudioSystem::AudioSystem::NEAudioIsValidGroup(std::string const& audio_group_tag)
{
	// Check if the group already exists in the map
	if (audio_components->audio_group_map.find(audio_group_tag) != audio_components->audio_group_map.end())
	{
		return true;
	}
	return false;
}

void AudioSystem::AudioSystem::NEAudioPlay(NE_AUDIO audio, NE_AUDIO_GROUP group, float vol, float pitch, bool loop)
{
	audio_components->fmod_system->playSound(audio, group, 0, nullptr);	
	// This is same as UNREFERENCED_PARAMETER
	static_cast<void>(vol);
	static_cast<void>(pitch);
	static_cast<void>(loop);
}

void AudioSystem::AudioSystem::NEAudioStopGroup(std::string const& tag)
{
	// This is same as UNREFERENCED_PARAMETER
	static_cast<void>(tag);
	if (true)
	{

	}
	audio_components->audio_group_map[tag]->stop();
}

bool AudioSystem::AudioSystem::NEAudioIsValid(NE_AUDIO_GROUP group)
{
	static_cast<void>(group);
	return false;
}

void AudioSystem::AudioSystem::NEAudioPauseGroup(NE_AUDIO_GROUP group)
{
	static_cast<void>(group);

}

void AudioSystem::AudioSystem::NEAudioResumeGroup(NE_AUDIO_GROUP group)
{
	static_cast<void>(group);
}

void AudioSystem::AudioSystem::NEAudioSetGroupPitch(NE_AUDIO_GROUP group, float pitch)
{
	static_cast<void>(group);
	static_cast<void>(pitch);
}

void AudioSystem::AudioSystem::NEAudioSetGroupVolume(NE_AUDIO_GROUP group)
{
	static_cast<void>(group);
}

void AudioSystem::AudioSystem::NEAudioUnloadGroup(NE_AUDIO_GROUP group)
{
	static_cast<void>(group);
}

void AudioSystem::AudioSystem::NEAudioUnloadAudio(NE_AUDIO audio)
{
	static_cast<void>(audio);
}

void AudioSystem::AudioSystem::IsPlaying(NE_AUDIO audio)
{
	static_cast<void>(audio);
}
