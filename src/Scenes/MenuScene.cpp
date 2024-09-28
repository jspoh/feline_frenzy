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
	//obj->setColor(Vector3(0.5f, 0.5f, 0.5f));
	NIKEEngine.registerComponent<Transform::Transform>();
	NIKEEngine.registerComponent<Render::Color>();
	//NIKEEngine.registerComponent<Camera::Camera>();

	//Add Input Singleton System
	NIKEEngine.registerSystem<Render::Manager>(Render::Manager::getInstance());

	//Add component types to system
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Transform::Transform>());
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Render::Color>());
	//NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Camera::Camera>());

	//Load models
	NIKEEngine.accessSystem<Render::Manager>()->registerModel("square", "assets/meshes/square.txt");
	NIKEEngine.accessSystem<Render::Manager>()->registerModel("triangle", "assets/meshes/triangle.txt");
}

void Menu::Scene::init() {
	glClearColor(1, 1, 1, 1);
	// init square obj with values
	//RenderManager::getInstance().createObject("obj1", "square");
	//Object* obj = RenderManager::getInstance().getObject("obj1");
	//obj->setScale(Vector2(0.8f, 0.8f));

	//Create entity
	std::vector<Entity::Type> entities;
	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(0), { "base", "square", {0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, Matrix33::Matrix_33()});
	NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(0), {{0.0f, 0.0f, 0.0f}, 1.0f});

	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(1), { "base", "triangle", {0.0f, 0.0f}, {0.5f, 1.0f}, 0.0f, Matrix33::Matrix_33() });
	NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(1), { {1.0f, 0.0f, 0.0f}, 1.0f });

	//entities.push_back(NIKEEngine.createEntity());
	//NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(2), Transform::Transform());
	//NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(2), Render::Color());

	//entities.push_back(NIKEEngine.createEntity());
	//NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(3), Transform::Transform());
	//NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(3), Render::Color());



	// Init game objects into game world
	//Render::Manager::getInstance().createObject("obj1", "square", Vector3(1.f, 0.f, 0.f), Vector2(-19800, -20000), Vector2(200.f, 150.f), 0.f, -17.5f);
	//Render::Manager::getInstance().createObject("obj2", "square", Vector3(0.f, 1.f, 0.f), Vector2(-19200, -20000), Vector2(225.f, 175.f), 0.f, 28.f);
	//Render::Manager::getInstance().createObject("obj3", "square", Vector3(0.5f, 1.f, 0.5f), Vector2(-19500, -19700), Vector2(225.f, 175.f), 0.f);
	//Render::Manager::getInstance().createObject("obj4", "square", Vector3(0.2f, 0.2f, 0.2f), Vector2(-19500, -20500), Vector2(225.f, 175.f), 0.f);
	//// Init camera object (player)
	//Render::Manager::getInstance().createObject("camera", "triangle", Vector3(0.f, 0.f, 0.f), Vector2(-19500, -19700), Vector2(50.f, 100.f), 180.f);
	////// Init camera
	//Render::Manager::getInstance().initCamera("camera");
}

void Menu::Scene::render() {

	//Object* obj = Render::Manager::getInstance().getObject("camera");
	//obj->setPosition(obj->getPosition().x, obj->getPosition().y - 1.f);


	//// Updates all object tranformation
	//Render::Manager::getInstance().updateObjects();

	//Render::Manager::getInstance().drawObjects();

}

void Menu::Scene::exit() {

}

void Menu::Scene::unload() {

}
