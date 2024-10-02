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
	//obj->setColor(Vector3(0.5f, 0.5f, 0.5f));
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

	//Register Shaders
	NIKEEngine.accessAssets()->registerShader("base", "shaders/base.vert", "shaders/base.frag");
	NIKEEngine.accessAssets()->registerShader("tex", "shaders/textured_rendering.vert", "shaders/textured_rendering.frag");

	//Register models
	NIKEEngine.accessAssets()->registerModel("square", "assets/meshes/square.txt");
	NIKEEngine.accessAssets()->registerModel("triangle", "assets/meshes/triangle.txt");
	NIKEEngine.accessAssets()->registerModel("circle", "assets/meshes/circle.txt");
	NIKEEngine.accessAssets()->registerModel("square-texture", "assets/meshes/square-texture.txt");

	//Register textures
	NIKEEngine.accessAssets()->registerTexture("duck", "assets/textures/duck-rgba-256.tex");
	NIKEEngine.accessAssets()->registerTexture("water", "assets/textures/water-rgba-256.tex");
	NIKEEngine.accessAssets()->registerTexture("tree", "assets/textures/tree.jpg");
	NIKEEngine.accessAssets()->registerTexture("ame", "assets/textures/ame.png");
}

void Menu::Scene::init() {
	glClearColor(1, 1, 1, 1);

	//Create entity
	std::vector<Entity::Type> entities;

	loadFromFile("assets/scenes/mainmenu.scn", entities);

	NIKEEngine.addEntityComponentObj<Move::Movement>(entities.at(1), {false, false, false, false});
	NIKEEngine.addEntityComponentObj<Transform::Velocity>(entities.at(1), { {0.0f, 0.0f} }); // Need Velocity component for Collider component currently
	Physics::Manager::getInstance()->collision_manager.setColliderComp(entities.at(1)); // Set collider for "player" entity (test collision)

	NIKEEngine.addEntityComponentObj<Transform::Velocity>(entities.at(2), { {0.0f, 1.0f} });


	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Render::Texture>(entities.at(3), { "duck",Matrix33::Matrix_33::Identity(), { {1.0f, 1.0f, 1.0f}, 1.0f }, { 1.0f / 2.0f, 1.0f / 2.0f}, {0.0f, 0.0f} });
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(3), { {400.0f, -200.0f}, {200.f, 200.f}, 0.0f });
	NIKEEngine.addEntityComponentObj<Transform::Runtime_Transform>(entities.at(3), Transform::Runtime_Transform());
	NIKEEngine.addEntityComponentObj<Transform::Velocity>(entities.at(3), { {0.0f, 0.0f} });
	Physics::Manager::getInstance()->collision_manager.setColliderComp(entities.at(3)); // Set collider for duck entity (test collision)
	//cout << "Duck bounding box min: " << NIKEEngine.getEntityComponent<Collision::Collider>(entities.at(3)).rect_min.x << ", " << NIKEEngine.getEntityComponent<Collision::Collider>(entities.at(3)).rect_min.y << endl;
	//cout << "Duck bounding box max: " << NIKEEngine.getEntityComponent<Collision::Collider>(entities.at(3)).rect_max.x << ", " << NIKEEngine.getEntityComponent<Collision::Collider>(entities.at(3)).rect_max.y << endl;

	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Render::Texture>(entities.at(4), { "water" ,Matrix33::Matrix_33::Identity(), { {1.0f, 1.0f, 1.0f}, 1.0f }, { 1.0f / 2.0f, 1.0f / 2.0f}, {0.0f, 0.0f} });
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(4), { {100.0f, -200.0f}, {200.f, 200.f}, 0.0f });

	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Render::Texture>(entities.at(5), { "tree" ,Matrix33::Matrix_33::Identity(), { {1.0f, 1.0f, 1.0f}, 1.0f }, { 1.0f / 2.0f, 1.0f / 2.0f}, {0.0f, 0.0f} });
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(5), { {-200.0f, -200.0f}, {200.f, 200.f}, 0.0f });
	NIKEEngine.addEntityComponentObj<Transform::Velocity>(entities.at(5), { {0.0f, 0.0f} });
	Physics::Manager::getInstance()->collision_manager.setColliderComp(entities.at(5)); // Set collider for tree entity (test collision)

	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Render::Texture>(entities.at(6), { "ame" ,Matrix33::Matrix_33::Identity(), { {1.0f, 1.0f, 1.0f}, 1.0f }, { 1.0f / 4.0f, 1.0f / 5.0f}, {0.0f, 0.0f} });
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(6), { {0.0f, 0.0f}, {500.f, 500.f}, 0.0f });
	NIKEEngine.addEntityComponentObj<Animation::cBase>(entities.at(6), Animation::cBase("AME-ANIMATOR", 1, 2.0f, true));
	NIKEEngine.addEntityComponentObj<Animation::cSprite>(entities.at(6), Animation::cSprite({ 4.0f, 5.0f }, { 0.0f, 0.0f }, {2.0f, 4.0f}));

	//Create object spawner
	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Input::Mouse>(entities.at(7), { Input::TriggerMode::TRIGGERED });
	NIKEEngine.addEntityComponentObj<GameLogic::ObjectSpawner>(entities.at(7), {});

	//Create camera
	NIKEEngine.addEntityComponentObj<Render::Cam>(entities.at(0), { "CAM1", {0.0f, 0.0f}, 1000.0f });
	NIKEEngine.addEntityComponentObj<Render::Cam>(entities.at(1), { "CAM2", {122.0f, 0.0f}, 1000.0f });

	NIKEEngine.accessSystem<Render::Manager>()->trackCamEntity("CAM2");
}

void Menu::Scene::exit() {

}

void Menu::Scene::unload() {

}

void Menu::Scene::loadFromFile(const std::string& scene_filepath, std::vector<Entity::Type>& entities) {
	Serialization::Manager serializationManager;
    serializationManager.loadSceneFromFile(scene_filepath, entities);
}
