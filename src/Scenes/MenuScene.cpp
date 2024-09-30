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
#include "../headers/Systems/sysPhysics.h"
#include "../headers/Systems/sysInput.h"

void Menu::Scene::load() {
	//obj->setColor(Vector3(0.5f, 0.5f, 0.5f));
	NIKEEngine.registerComponent<Transform::Velocity>();
	NIKEEngine.registerComponent<Transform::Transform>();
	NIKEEngine.registerComponent<Move::Movement>();
	NIKEEngine.registerComponent<Render::Mesh>();
	NIKEEngine.registerComponent<Render::Color>();
	NIKEEngine.registerComponent<Render::Cam>();

	//Add Input Singleton System
	NIKEEngine.registerSystem<Render::Manager>(Render::Manager::getInstance());
	NIKEEngine.accessSystem<Render::Manager>()->setComponentsLinked(false);
	NIKEEngine.registerSystem<Physics::Manager>(Physics::Manager::getInstance());
	NIKEEngine.accessSystem<Physics::Manager>()->setComponentsLinked(false);

	//Add component types to system
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Transform::Transform>());
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Render::Mesh>());
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Render::Color>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Transform::Velocity>());
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Move::Movement>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Move::Movement>());

	//Load models
	NIKEEngine.accessSystem<Render::Manager>()->registerModel("square", "assets/meshes/square.txt");
	NIKEEngine.accessSystem<Render::Manager>()->registerModel("triangle", "assets/meshes/triangle.txt");
	NIKEEngine.accessSystem<Render::Manager>()->registerModel("circle", "assets/meshes/circle.txt");
}

void Menu::Scene::init() {
	glClearColor(1, 1, 1, 1);

	//Create entity
	std::vector<Entity::Type> entities;

	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Render::Mesh>(entities.at(0), { "base", "square", Matrix33::Matrix_33()});
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(0), {{0.0f, 0.0f}, {225.f, 175.f}, 0.0f, false});
	NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(0), {{0.0f, 0.0f, 0.0f}, 1.0f});
	NIKEEngine.addEntityComponentObj<Render::Cam>(entities.at(0), {"CAM1", {0.0f, 0.0f}, 1000.0f });

	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Render::Mesh>(entities.at(1), { "base", "triangle", Matrix33::Matrix_33() });
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(1), {{122.0f, 0.0f}, {50.f, 100.f}, 0.0f, true });
	NIKEEngine.addEntityComponentObj<Transform::Velocity>(entities.at(1), { {0.0f, 0.0f} });
	NIKEEngine.addEntityComponentObj<Move::Movement>(entities.at(1), {false, false, false, false});
	NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(1), { {1.0f, 0.0f, 0.0f}, 1.0f });
	NIKEEngine.addEntityComponentObj<Render::Cam>(entities.at(1), {"CAM2", {122.0f, 0.0f}, 1000.0f });

	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Render::Mesh>(entities.at(2), { "base", "circle", Matrix33::Matrix_33() });
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(2), { {322.0f, 122.0f}, {100.f, 100.f}, 0.0f, false });
	NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(2), { {1.0f, 0.0f, 1.0f}, 1.0f });

	//entities.push_back(NIKEEngine.createEntity());
	//NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(3), Transform::Transform());
	//NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(3), Render::Color());


	NIKEEngine.accessSystem<Render::Manager>()->trackCamEntity("CAM2");

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

void Menu::Scene::exit() {

}

void Menu::Scene::unload() {

}
