
#include "stdafx.h"
#include "AudioSystem.h"

// Create Fmod instance
AudioSystem::AudioSystem::AudioSystem()
{
	FMOD::System_Create(&audio_components->fmod_system);
}

void AudioSystem::AudioSystem::init()
{
	audio_components->fmod_system->init(32, FMOD_INIT_NORMAL, nullptr);
}

void AudioSystem::AudioSystem::update()
{

}

// Release Fmod system
AudioSystem::AudioSystem::~AudioSystem()
{
	// Release Fmod
	audio_components->fmod_system->release();
}