/*****************************************************************//**
 * \file   MenuScene.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \co-author Sean Gwee, g.boonxuensean@digipen.edu, 
 *\ co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu
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

	//Register textures
	NIKEEngine.accessAssets()->registerTexture("duck", "assets/textures/duck-rgba-256.tex");
	NIKEEngine.accessAssets()->registerTexture("tree", "assets/textures/Tree_Orange.png");
	NIKEEngine.accessAssets()->registerTexture("player", "assets/textures/player.png");
	NIKEEngine.accessAssets()->registerTexture("background", "assets/textures/Tileset_Guide.png");

	//Register game logic components
	NIKEEngine.registerComponent<GameLogic::ObjectSpawner>();

	//Register game logic system
	NIKEEngine.registerSystem<GameLogic::Manager>();
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<GameLogic::ObjectSpawner>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Input::Key>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Input::Mouse>());
	NIKEEngine.accessSystem<GameLogic::Manager>()->setComponentsLinked(false);
}

void Menu::Scene::init() {
	glClearColor(1, 1, 1, 1);

	NIKEEngine.accessSeri()->loadSceneFromFile("assets/scenes/mainmenu.scn", entities);

	//Create new scene object
	entities["next_scene"] = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Scenes::ChangeScene>(entities["next_scene"], { Scenes::Actions::CHANGE, "PERFORMANCE", -1, GLFW_KEY_ENTER });
	NIKEEngine.addEntityComponentObj<Input::Key>(entities["next_scene"], { Input::TriggerMode::TRIGGERED });

	// Adding rotation control
	NIKEEngine.addEntityComponentObj<Transform::Runtime_Transform>(entities["tree"], Transform::Runtime_Transform());
	NIKEEngine.accessSystem<Render::Manager>()->trackCamEntity("background");

	//Create object spawner
	Entity::Type objSpawner = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Mouse>(objSpawner, { Input::TriggerMode::TRIGGERED });
	NIKEEngine.addEntityComponentObj<GameLogic::ObjectSpawner>(objSpawner, {});

	//Toggle debug mode
	Entity::Type debugMode = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Mouse>(debugMode, { Input::TriggerMode::TRIGGERED });

	//Toggle debug mode
	Entity::Type key = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Key>(key, { Input::TriggerMode::TRIGGERED });

	//Create text object
	Entity::Type basic_text = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Render::Text>(basic_text, { "basic", "HELLO WORLD", {{0.0f, 0.0f, 0.0f}, 1.0f}, {0.4f, 0.9f}, 0.05f });

	Entity::Type pantat_text = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Render::Text>(pantat_text, { "basic", "PANTAT", {{0.0f, 0.0f, 0.0f}, 1.0f}, {0.5f, -0.9f}, 0.05f });
}

void Menu::Scene::exit() {
	NIKEEngine.destroyAllEntities();
}

void Menu::Scene::unload() {

}
