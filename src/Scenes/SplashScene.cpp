/*****************************************************************//**
 * \file   SplashScene.cpp
 * \brief
 *
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/SplashScene.h"
#include "../headers/Systems/sysAudio.h"
#include "../headers/Systems/sysInput.h"

void Splash::Scene::load() {

}

void Splash::Scene::init() {

	//Set BG Color
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	//Register input component
	NIKEEngine.registerComponent<Input::Mouse>();
	NIKEEngine.registerComponent<Input::Key>();
	NIKEEngine.registerComponent<Audio::cAudio>();

	//Add Input Singleton System
	NIKEEngine.registerSystem<Input::Manager>(Input::Manager::getInstance());
	NIKEEngine.registerSystem<Audio::Manager>(Audio::Manager::getInstance());

	NIKEEngine.accessSystem<Input::Manager>()->setComponentsLinked(false);

	//Add component types to system
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Mouse>());
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Key>());

	NIKEEngine.addSystemComponentType<Audio::Manager>(NIKEEngine.getComponentType<Audio::cAudio>());

	//Create entity with mouse component
	mouse_click = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Mouse>(mouse_click, Input::Mouse());
	NIKEEngine.addEntityComponentObj<Input::Key>(mouse_click, Input::Key());

	// Create entity to test for audio
	bg_music = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Audio::cAudio>(bg_music, Audio::cAudio());

	// Load music files with asset manager
	ASSET_MANAGER.LoadMusic("assets/Audio/test_music.wav", "test_music");
	ASSET_MANAGER.CreateAudioGroup("test_group");
}

void Splash::Scene::render() {
	//For testing
	if (NIKEEngine.getEntityComponent<Input::Mouse>(mouse_click).is_lbutton_pressed) {
		NIKEEngine.changeScene("MENU");
		NIKEEngine.accessSystem<Audio::Manager>()->NEAudioStopGroup(ASSET_MANAGER.GetAudioGroup("test_group"));
	}
}

void Splash::Scene::exit() {

}

void Splash::Scene::unload() {

}
