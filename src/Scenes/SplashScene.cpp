/*****************************************************************//**
 * \file   SplashScene.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/SplashScene.h"
#include "../headers/Systems/sysAudio.h"
#include "../headers/Systems/sysInput.h"
#include "../headers/Components/cAudio.h"
#include "../headers/Systems/sysGameLogic.h"
#include "../headers/Components/cScene.h"

void Splash::Scene::load() {

}

void Splash::Scene::init() {

	//Set BG Color
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	//Register input component
	NIKEEngine.registerComponent<Input::Key>();
	NIKEEngine.registerComponent<Input::Mouse>();
	NIKEEngine.registerComponent<Audio::cAudio>();
	NIKEEngine.registerComponent<Scenes::ChangeSceneEvent>();
	NIKEEngine.registerComponent<GameLogic::ObjectSpawner>();

	//Add Input Singleton System
	NIKEEngine.registerSystem<Input::Manager>(Input::Manager::getInstance());
	NIKEEngine.accessSystem<Input::Manager>()->setComponentsLinked(false);

	//Add component types to system
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Key>());
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Mouse>());

	//Register audio system
	NIKEEngine.registerSystem<Audio::Manager>(Audio::Manager::getInstance());

	//Add audio component 
	NIKEEngine.addSystemComponentType<Audio::Manager>(NIKEEngine.getComponentType<Audio::cAudio>());

	//Register game logic system
	NIKEEngine.registerSystem<GameLogic::Manager>();
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Scenes::ChangeSceneEvent>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<GameLogic::ObjectSpawner>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Input::Key>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Input::Mouse>());
	NIKEEngine.accessSystem<GameLogic::Manager>()->setComponentsLinked(false);

	//Create entity with mouse & key component for testing
	input = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Key>(input, { Input::TriggerMode::TRIGGERED });
	NIKEEngine.addEntityComponentObj<Input::Mouse>(input, { Input::TriggerMode::TRIGGERED });

	// Load music files with asset manager
	NIKEEngine.accessAssets()->registerMusicAudio("assets/Audio/test_music.wav", "test_music");
	NIKEEngine.accessAssets()->createAudioGroup("test_group");

	//// Create entity to test for audio
	bg_music = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Audio::cAudio>(bg_music, Audio::cAudio(NIKEEngine.accessAssets()->getAudio("test_music"), NIKEEngine.accessAssets()->getAudioGroup("test_group"), 1.f, 1.f, false));

	next_scene = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Scenes::ChangeSceneEvent>(next_scene, { Scenes::Actions::CHANGE, "MENU" });
	NIKEEngine.addEntityComponentObj<Input::Mouse>(next_scene, { Input::TriggerMode::TRIGGERED });
}

void Splash::Scene::exit() {

}

void Splash::Scene::unload() {

}
