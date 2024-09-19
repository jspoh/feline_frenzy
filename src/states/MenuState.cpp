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
	RenderManager::getInstance().registerModel("square", "assets/meshes/square.txt");
	RenderManager::getInstance().registerModel("triangle", "assets/meshes/triangle.txt");
}

void MenuState::init() {
}

void MenuState::update() {
}

void MenuState::render() {
	Object obj("square");
	obj.draw();
}

void MenuState::exit() {
}

void MenuState::unload() {
}
