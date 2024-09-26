/*****************************************************************//**
 * \file   MenuScene.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/MenuScene.h"
#include "../headers/Systems/Render/sysRender.h"

void Menu::Scene::load() {
	Render::Manager::getInstance().registerModel("square", "assets/meshes/square.txt");
	Render::Manager::getInstance().registerModel("triangle", "assets/meshes/triangle.txt");
}

void Menu::Scene::init() {
	// init square obj with values
	//RenderManager::getInstance().createObject("obj1", "square");
	//Object* obj = RenderManager::getInstance().getObject("obj1");
	//obj->setScale(Vector2(0.8f, 0.8f));

	//obj->setColor(Vector3(0.5f, 0.5f, 0.5f));

	// Init game objects into game world
	Render::Manager::getInstance().createObject("obj1", "square", Vector3(1.f, 0.f, 0.f), Vector2(-19800, -20000), Vector2(200.f, 150.f), 0.f, -17.5f);
	Render::Manager::getInstance().createObject("obj2", "square", Vector3(0.f, 1.f, 0.f), Vector2(-19200, -20000), Vector2(225.f, 175.f), 0.f, 28.f);
	Render::Manager::getInstance().createObject("obj3", "square", Vector3(0.5f, 1.f, 0.5f), Vector2(-19500, -19700), Vector2(225.f, 175.f), 0.f);
	Render::Manager::getInstance().createObject("obj4", "square", Vector3(0.2f, 0.2f, 0.2f), Vector2(-19500, -20500), Vector2(225.f, 175.f), 0.f);
	// Init camera object (player)
	Render::Manager::getInstance().createObject("camera", "triangle", Vector3(0.f, 0.f, 0.f), Vector2(-19500, -19700), Vector2(50.f, 100.f), 180.f);
	//// Init camera
	Render::Manager::getInstance().initCamera("camera");
}

void Menu::Scene::render() {
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	Object* obj = Render::Manager::getInstance().getObject("camera");
	obj->setPosition(obj->getPosition().x, obj->getPosition().y - 1.f);


	// Updates all object tranformation
	Render::Manager::getInstance().updateObjects();

	Render::Manager::getInstance().drawObjects();

}

void Menu::Scene::exit() {

}

void Menu::Scene::unload() {

}
