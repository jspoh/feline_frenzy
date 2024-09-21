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
	
	// init triangle obj with values
	RenderManager::getInstance().createObject("obj2", "triangle", Vector2(0.5,0.5), Vector2(0.5,0.5));
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
