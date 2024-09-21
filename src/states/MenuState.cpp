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
#include "Mtx33.h"



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
	Matrix33::Matrix_33 test{}, test_scale{};
	Matrix33::Matrix_33Translate(test,6,7);
	Matrix33::Matrix_33Scale(test_scale, 3, 5);
	Matrix33::Matrix_33 result = test * test_scale;
	cout << result << endl;
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
