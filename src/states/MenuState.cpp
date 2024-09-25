/*****************************************************************//**
 * \file   MenuState.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/


#include "../headers/Core/stdafx.h"
#include "../headers/States/MenuState.h"
#include "../headers/Managers/mState.h"
#include "../headers/Systems/sysRender.h"


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
	RenderManager::getInstance().registerMesh("square", "assets/meshes/square.txt");
}

void MenuState::init() {
}

void MenuState::update() {
}

void MenuState::render() {
	RenderManager::getInstance().drawModel("square");
}

void MenuState::exit() {
}

void MenuState::unload() {
}
