///*****************************************************************//**
// * \file   SplashScene.cpp
// * \brief
// * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (90%)
// * \co-author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (10%)
// * \date   September 2024
// * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
// *********************************************************************/

#include "Core/pch.h"
#include "Scenes/SplashScene.h"

void Splash::Scene::load() {
	//Load music
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadMusic("assets/Audio/test_music.wav", "MUSIC");
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadSound("assets/Audio/test_sound.wav", "SFX");
}

void Splash::Scene::init() {
	//!TODO SET BACKGROUND COLOR IN RENDER MANAGER
	//glClearColor(0, 0, 0, 0);
	//glClear(GL_COLOR_BUFFER_BIT);

	//Creat Channel Group
	NIKEEngine.getService<NIKESAURUS::Audio::Service>()->createChannelGroup("MASTER");

	//Play Music
	NIKEEngine.getService<NIKESAURUS::Audio::Service>()->playAudio("MUSIC", "MUSIC_CHANNEL", "MASTER", 1.0f, 1.0f, true);

	//Set Music Volume
	NIKEEngine.getService<NIKESAURUS::Audio::Service>()->getChannelGroup("MASTER")->setVolume(0.5f);

	// Test Logger
	NIKEE_WARN("This is a warning message");

	//Create entity
	NIKESAURUS::Entity::Type sfx_1 = NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->createEntity();
	NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Audio::SFX>(sfx_1, NIKESAURUS::Audio::SFX(true, "SFX", "MASTER", 0.5f, 1.0f));
	
	// Test crash logger
	LOG_CRASH("This is a test crash");
}

void Splash::Scene::exit() {

}

void Splash::Scene::unload() {

}
