/*****************************************************************//**
 * \file   MenuScene.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/MenuScene.h"
#include "../headers/Systems/sysRender.h"

void Menu::Scene::load() {
	RenderManager::getInstance().registerMesh("square", "assets/meshes/square.txt");
}

void Menu::Scene::init() {

}

void Menu::Scene::render() {
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	RenderManager::getInstance().drawModel("square");
}

void Menu::Scene::exit() {

}

void Menu::Scene::unload() {

}
