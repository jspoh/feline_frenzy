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


#define PI 3.14F

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
	
	// init triangle obj with values
	RenderManager::getInstance().createObject("obj2", "triangle", Vector2(0.5,0.5), Vector2(0.5,0.5));
	
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
	RenderManager::getInstance().updateObjects();
}

void MenuState::render() {
	RenderManager::getInstance().drawObjects();
}

void MenuState::exit() {
}

void MenuState::unload() {
}
