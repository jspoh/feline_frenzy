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
	//RenderManager::getInstance().createObject("obj1", "square");
	//Object* obj = RenderManager::getInstance().getObject("obj1");
	//obj->setScale(Vector2(0.8f, 0.8f));

	//obj->setColor(Vector3(0.5f, 0.5f, 0.5f));

	// Init game objects into game world
	RenderManager::getInstance().createObject("obj1", "square", Vector3(1.f, 0.f, 0.f), Vector2(-19800, -20000), Vector2(200.f,150.f), 0.f , -17.5f);
	RenderManager::getInstance().createObject("obj2", "square", Vector3(0.f, 1.f, 0.f), Vector2(-19200, -20000), Vector2(225.f, 175.f), 0.f, 28.f);
	RenderManager::getInstance().createObject("obj3", "square", Vector3(0.5f, 1.f, 0.5f), Vector2(-19500, -19700), Vector2(225.f, 175.f), 0.f);
	RenderManager::getInstance().createObject("obj4", "square", Vector3(0.2f, 0.2f, 0.2f), Vector2(-19500, -20500), Vector2(225.f, 175.f), 0.f);
	// Init camera object (player)
	RenderManager::getInstance().createObject("camera", "triangle", Vector3(0.f, 0.f, 0.f), Vector2(-19500, -19700), Vector2(50.f, 100.f), 180.f);
	// Init camera
	RenderManager::getInstance().initCamera("camera");

}

void MenuState::update() {
	Object* obj = RenderManager::getInstance().getObject("camera");
	obj->setPosition(obj->getPosition().x, obj->getPosition().y - 1.f);


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
