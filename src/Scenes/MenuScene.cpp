/*****************************************************************//**
 * \file   MenuScene.cpp
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/MenuScene.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Systems/sysPhysics.h"
#include "../headers/Managers/mSerialization.h"
#include "../headers/Systems/sysGameLogic.h"
#include "../headers/Systems/Animation/sysAnimation.h"
//!TODO Clean up scene parser

void Menu::Scene::load() {
	// Register required components
	NIKEEngine.registerComponent<Transform::Velocity>();
	NIKEEngine.registerComponent<Transform::Transform>();
	NIKEEngine.registerComponent<Transform::Runtime_Transform>();
	NIKEEngine.registerComponent<Move::Movement>();
	NIKEEngine.registerComponent<Render::Shape>();
	NIKEEngine.registerComponent<Render::Texture>();
	NIKEEngine.registerComponent<Render::Color>();
	NIKEEngine.registerComponent<Render::Cam>();
	NIKEEngine.registerComponent<Collision::Collider>(); // Under sysPhysics
	NIKEEngine.registerComponent<Animation::cBase>();
	NIKEEngine.registerComponent<Animation::cSprite>();

	//Add Singleton System
	NIKEEngine.registerSystem<Physics::Manager>(Physics::Manager::getInstance());
	NIKEEngine.registerSystem<Animation::Manager>();
	NIKEEngine.registerSystem<Render::Manager>(Render::Manager::getInstance());

	// Set components link
	NIKEEngine.accessSystem<Physics::Manager>()->setComponentsLinked(false);
	NIKEEngine.accessSystem<Render::Manager>()->setComponentsLinked(false);

	//Add component types to system
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Transform::Runtime_Transform>());
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Move::Movement>());

	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Transform::Velocity>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Transform::Runtime_Transform>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Transform::Transform>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Move::Movement>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType <Collision::Collider>()); // Under sysPhysics

	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Transform::Transform>());
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Render::Shape>());
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Render::Texture>());

	NIKEEngine.addSystemComponentType<Animation::Manager>(NIKEEngine.getComponentType<Animation::cBase>());
	NIKEEngine.addSystemComponentType<Animation::Manager>(NIKEEngine.getComponentType<Animation::cSprite>());
	NIKEEngine.addSystemComponentType<Animation::Manager>(NIKEEngine.getComponentType<Render::Texture>());

	//Add event listener for animation system
	NIKEEngine.accessEvents()->addEventListeners<Animation::AnimationEvent>(NIKEEngine.accessSystem<Animation::Manager>());

	//Register textures
	NIKEEngine.accessAssets()->registerTexture("duck", "assets/textures/duck-rgba-256.tex");
	NIKEEngine.accessAssets()->registerTexture("water", "assets/textures/water-rgba-256.tex");
	NIKEEngine.accessAssets()->registerTexture("tree", "assets/textures/tree.jpg");
	NIKEEngine.accessAssets()->registerTexture("ame", "assets/textures/ame.png");
}

void Menu::Scene::init() {
	glClearColor(1, 1, 1, 1);

	loadFromFile("assets/scenes/mainmenu.scn", entities);

	// Adding rotation control
	NIKEEngine.addEntityComponentObj<Transform::Runtime_Transform>(entities["duckobj"], Transform::Runtime_Transform());

	// Adding another possible camera object
	NIKEEngine.addEntityComponentObj<Render::Cam>(entities["obj1"], { "CAM1", {0.0f, 0.0f}, 1000.0f });

	//Create object spawner
	Entity::Type objSpawner = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Mouse>(objSpawner, { Input::TriggerMode::TRIGGERED });
	NIKEEngine.addEntityComponentObj<GameLogic::ObjectSpawner>(objSpawner, {});

}

void Menu::Scene::exit() {

}

void Menu::Scene::unload() {

}

void Menu::Scene::loadFromFile(const std::string& scene_filepath, std::unordered_map<std::string, Entity::Type>& param_entities) {
	Serialization::Manager serializationManager;
    serializationManager.loadSceneFromFile(scene_filepath, param_entities);
}
