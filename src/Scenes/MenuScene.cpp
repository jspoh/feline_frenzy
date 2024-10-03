/*****************************************************************//**
 * \file   MenuScene.cpp
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \co-author Sean Gwee, g.boonxuensean@digipen.edu, Min Khant Ko, 2301320, ko.m@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/MenuScene.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Systems/Physics/sysPhysics.h"
#include "../headers/Managers/mSerialization.h"
#include "../headers/Systems/GameLogic/sysGameLogic.h"
#include "../headers/Systems/Animation/sysAnimation.h"
//!TODO Clean up scene parser

void Menu::Scene::load() {
	// Register required components
	NIKEEngine.registerComponent<Transform::Transform>();
	NIKEEngine.registerComponent<Transform::Runtime_Transform>();
	NIKEEngine.registerComponent<Render::Shape>();
	NIKEEngine.registerComponent<Render::Texture>();
	NIKEEngine.registerComponent<Render::Color>();
	NIKEEngine.registerComponent<Render::Cam>();
	NIKEEngine.registerComponent<Render::Text>();
	NIKEEngine.registerComponent<Collision::Collider>(); // Under sysPhysics
	NIKEEngine.registerComponent<Animation::cBase>();
	NIKEEngine.registerComponent<Animation::cSprite>();

	//Add Singleton System
	NIKEEngine.registerSystem<Physics::Manager>(Physics::Manager::getInstance());
	NIKEEngine.registerSystem<Animation::Manager>();
	NIKEEngine.registerSystem<Render::Manager>(Render::Manager::getInstance());

	//Add component types to system
	//NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Transform::Runtime_Transform>());
	//NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Move::Movement>());

	//Register textures
	NIKEEngine.accessAssets()->registerTexture("duck", "assets/textures/duck-rgba-256.tex");
	NIKEEngine.accessAssets()->registerTexture("water", "assets/textures/water-rgba-256.tex");
	NIKEEngine.accessAssets()->registerTexture("tree", "assets/textures/tree.jpg");
	NIKEEngine.accessAssets()->registerTexture("ame", "assets/textures/ame.png");
	NIKEEngine.accessAssets()->registerTexture("player", "assets/textures/player.png");
	NIKEEngine.accessAssets()->registerTexture("background", "assets/textures/Tileset_Guide.png");
}

void Menu::Scene::init() {
	glClearColor(1, 1, 1, 1);

	NIKEEngine.accessSeri()->loadSceneFromFile("assets/scenes/mainmenu.scn", entities);

	// Adding rotation control
	//NIKEEngine.addEntityComponentObj<Transform::Runtime_Transform>(entities["obj4"], Transform::Runtime_Transform());

	//Create object spawner
	Entity::Type objSpawner = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Mouse>(objSpawner, { Input::TriggerMode::TRIGGERED });
	NIKEEngine.addEntityComponentObj<GameLogic::ObjectSpawner>(objSpawner, {});

	//Toggle debug mode
	Entity::Type debugMode = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Mouse>(debugMode, { Input::TriggerMode::TRIGGERED });

	//Create text object
	 Entity::Type basic_text = NIKEEngine.createEntity();
	 NIKEEngine.addEntityComponentObj<Render::Text>(basic_text, { "basic", "A", {{0.0f, 0.0f, 0.0f}, 1.0f}, {0.0f, 0.0f}, 1.0f });
}

void Menu::Scene::exit() {

}

void Menu::Scene::unload() {

}
