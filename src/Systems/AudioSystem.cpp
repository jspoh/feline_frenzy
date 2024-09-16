
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
}

void AudioSystem::AudioSystem::init()
{
	FMOD_RESULT result;

	result = audio_components->fmod_system->init(512, FMOD_STUDIO_INIT_NORMAL, 0);
	// Check if fmod studio system is initialized
	if (result != FMOD_OK)
	{
		cerr << "INITIALIZATION OF STUDIO FMOD SYSTEM FAILED!" << endl;
		exit(-1);
	}
}

void AudioSystem::AudioSystem::update()
{
	audio_components->fmod_system->update();
}

// Release Fmod system
AudioSystem::AudioSystem::~AudioSystem()
{
	// Release fmod studio
	audio_components->fmod_system->release();
}

NE_AUDIO AudioSystem::AudioSystem::NEAudioLoadSound(std::string& file_path, NE_AUDIO_GROUP audio_group)
{
	std::uint32_t file_size;
	FMOD::Sound* temp = nullptr;
	FMOD_RESULT result;
	// Create sound to be pushed into container
	result = audio_components->fmod_system->createSound(file_path.c_str(), FMOD_OPENONLY, nullptr, &temp);

	// Check for audio file validadity
	if (result != FMOD_OK)
	{
		temp->release();
		return nullptr;
	}
	// For setting buffer size
	temp->getLength(&file_size, FMOD_TIMEUNIT_RAWBYTES);
	audio_components->file_buffer = new char[file_size];

	result = temp->readData(audio_components->file_buffer, file_size, nullptr);

	// Check for audio file validadity
	if(result != FMOD_OK) 
	{
		temp->release(); 
		return nullptr;
	}

	// Push sound after reading from file
	audio_components->audio_map[audio_group] = temp;

	// Clear buffer
	delete[] audio_components->file_buffer;

	return temp;
}

NE_AUDIO AudioSystem::AudioSystem::NEAudioLoadMusic(std::string& file_path, NE_AUDIO_GROUP audio_group)
{
	FMOD::Sound* temp = nullptr;
	FMOD_RESULT result;
	// Create sound to be pushed into container
	// !!! FMOD will handle the reading from file using createStream function
	result = audio_components->fmod_system->createStream(file_path.c_str(), FMOD_CREATESTREAM, nullptr, &temp);
	
	// Check for audio file validadity
	if (result != FMOD_OK || temp == nullptr)
	{
		return nullptr;
	}

	// Push sound after reading from file
	audio_components->audio_map[audio_group] = temp;

	return temp;
}
