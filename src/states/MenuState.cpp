/*****************************************************************//**
 * \file   MenuState.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/


#include "stdafx.h"
#include "MenuState.h"
#include "StateManager.h"
#include "RenderManager.h"




MenuState::MenuState() {
	StateManager::getInstance().register_state("main_menu", this);
}

MenuState::~MenuState() {
}

MenuState& MenuState::getInstance() {
	static MenuState instance;
	return instance;
}

void MenuState::load() {
	// load meshes
	RenderManager::getInstance().registerModel("square", "assets/meshes/square.txt");
	RenderManager::getInstance().registerModel("triangle", "assets/meshes/triangle.txt");
}

void MenuState::init() {

	// init square obj with values
	RenderManager::getInstance().createObject("obj1", "square");
	Object* obj = RenderManager::getInstance().getObject("obj1");
	obj->setScale(Vector2(0.8f, 0.8f));
	obj->setRot(1.f);
	obj->setColor(Vector3(0.5f, 0.5f, 0.5f));
	
	// init triangle obj with values
	RenderManager::getInstance().createObject("obj2", "triangle",Vector3(0.f,1.f,1.f), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), 45.f);
	
	// Init camera (player)
	RenderManager::getInstance().createObject("camera", "triangle", Vector3(0.f, 0.f, 0.f), Vector2(0.f, 0.f), Vector2(0.125f, 0.25f));

	Matrix33::Matrix_33 test{}, test_scale{}, test_rot{}, temp{};
	Matrix33::Matrix_33Translate(test,6,7);
	Matrix33::Matrix_33Scale(test_scale, 3, 5);
	Matrix33::Matrix_33Rot(test_rot, PI);
	Matrix33::Matrix_33 result = test * test_scale;
	temp = result;
	Matrix33::Matrix_33Inverse(result, temp);
	cout << result << endl;
}

void MenuState::update() {
	Object* obj = RenderManager::getInstance().getObject("camera");
	if (obj->getPosition().y <= 1) {
		obj->setPosition(0, obj->getPosition().y + 0.005f);
	}

	// Updates all object tranformation
	RenderManager::getInstance().updateObjects();
}

void MenuState::render() {
	RenderManager::getInstance().drawObjects();
}

void MenuState::exit() {
}

void MenuState::unload() {
}
