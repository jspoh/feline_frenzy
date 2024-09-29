/*****************************************************************//**
 * \file   SplashScene.cpp
 * \brief
 *
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/SplashScene.h"
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

	//Add Input Singleton System
	NIKEEngine.registerSystem<Input::Manager>(Input::Manager::getInstance());
	NIKEEngine.accessSystem<Input::Manager>()->setComponentsLinked(false);

	//Add component types to system
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Mouse>());
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Key>());

	//Create entity with mouse component
	mouse_click = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Mouse>(mouse_click, Input::Mouse());
	NIKEEngine.addEntityComponentObj<Input::Key>(mouse_click, Input::Key());
}

void Splash::Scene::exit() {

}

void Splash::Scene::unload() {

}
