/*****************************************************************//**
 * \file   MenuScene.cpp
 * \brief
 *
 * \co-author Sean Gwee, g.boonxuensean@digipen.edu (50%)
 *\ co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (50%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/MenuScene.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Systems/Physics/sysPhysics.h"
#include "../headers/Managers/mSerialization.h"
#include "../headers/Systems/GameLogic/sysGameLogic.h"
#include "../headers/Systems/Animation/sysAnimation.h"
#include "../headers/Systems/GameLogic/sysObjectSpawner.h"
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
	NIKEEngine.registerSystem<GameLogic::Manager>(nullptr, NIKEEngine.getSystemIndex<Physics::Manager>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<GameLogic::ObjectSpawner>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Input::Key>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Input::Mouse>());
	NIKEEngine.accessSystem<GameLogic::Manager>()->setComponentsLinked(false);
}

void Menu::Scene::init() {
	glClearColor(1, 1, 1, 1);

	//Load objects from scene file
	NIKEEngine.accessSeri()->loadSceneFromFile("assets/scenes/mainmenu.scn", entities);

	// Adding rotation control
	NIKEEngine.addEntityComponentObj<Transform::Runtime_Transform>(entities["tree"], Transform::Runtime_Transform());
	NIKEEngine.accessSystem<Render::Manager>()->trackCamEntity("background");

	//Create object spawner
	Entity::Type objSpawner = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Mouse>(objSpawner, { Input::TriggerMode::TRIGGERED });
	NIKEEngine.addEntityComponentObj<GameLogic::ObjectSpawner>(objSpawner, GameLogic::ObjectSpawner());

	//Toggle debug mode
	Entity::Type key = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Key>(key, { Input::TriggerMode::TRIGGERED });
	NIKEEngine.addEntityComponentObj<Render::Debug>(key, Render::Debug());

	//Create text object
	Entity::Type basic_text = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Render::Text>(basic_text, { "basic", "HELLO WORLD", {{0.0f, 0.0f, 0.0f}, 1.0f}, {0.4f, 0.9f}, 0.05f });

	Entity::Type pantat_text = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Render::Text>(pantat_text, { "basic", "TEXT", {{0.0f, 0.0f, 0.0f}, 1.0f}, {0.5f, -0.9f}, 0.05f });
}

void Menu::Scene::exit() {
	NIKEEngine.destroyAllEntities();

	//Remove component and systems that are not in use
	NIKEEngine.removeComponent<GameLogic::ObjectSpawner>();
	NIKEEngine.removeSystem<GameLogic::Manager>();
}

void Menu::Scene::unload() {

}
