/*****************************************************************//**
 * \file   Engine.cpp
 * \brief  Core engine arhcitecture
 * 
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Core/Engine.h"

//Registerd Components
#include "../headers/Components/cInput.h"
#include "../headers/Components/cRender.h"
#include "../headers/Components/cAudio.h"
#include "../headers/Components/cMove.h"

 //Registered Systems
#include "../headers/Systems/sysInput.h"
#include "../headers/Systems/Physics/sysPhysics.h"
#include "../headers/Systems/Animation/sysAnimation.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Systems/sysAudio.h"
#include "../headers/Systems/GameLogic/sysObjectSpawner.h"

namespace NIKESAURUS {
	Core::Engine::~Engine() {
		glfwTerminate();
	}

	void Core::Engine::registerDefComponents() {
		////Register input components
		//ecs_coordinator->registerComponent<Input::Key>();
		//ecs_coordinator->registerComponent<Input::Mouse>();

		////Register physics components
		//ecs_coordinator->registerComponent<Transform::Velocity>();
		//ecs_coordinator->registerComponent<Transform::Transform>();
		//ecs_coordinator->registerComponent<Transform::Runtime_Transform>();
		//ecs_coordinator->registerComponent<Move::Movement>();
		//ecs_coordinator->registerComponent<Collision::Collider>();

		////Register animation components
		//ecs_coordinator->registerComponent<Animation::cBase>();
		//ecs_coordinator->registerComponent<Animation::cSprite>();

		////Register render components
		//ecs_coordinator->registerComponent<Render::Shape>();
		//ecs_coordinator->registerComponent<Render::Texture>();
		//ecs_coordinator->registerComponent<Render::Color>();
		//ecs_coordinator->registerComponent<Render::Cam>();
		//ecs_coordinator->registerComponent<Render::Text>();
		//ecs_coordinator->registerComponent<Render::Debug>();

		////Register audio components
		//ecs_coordinator->registerComponent<Audio::cAudio>();
	}

	void Core::Engine::registerDefSystems() {
		////Register input manager
		//ecs_coordinator->registerSystem<Input::Manager>(Input::Manager::getInstance());
		//ecs_coordinator->accessSystem<Input::Manager>()->setComponentsLinked(false);
		//ecs_coordinator->addSystemComponentType<Input::Manager>(getComponentType<Input::Key>());
		//ecs_coordinator->addSystemComponentType<Input::Manager>(getComponentType<Input::Mouse>());
		//ecs_coordinator->addSystemComponentType<Input::Manager>(getComponentType<Transform::Runtime_Transform>());

		////Register physics manager
		//ecs_coordinator->registerSystem<Physics::Manager>(Physics::Manager::getInstance());
		//ecs_coordinator->accessSystem<Physics::Manager>()->setComponentsLinked(false);
		//ecs_coordinator->addSystemComponentType<Physics::Manager>(getComponentType<Transform::Velocity>());
		//ecs_coordinator->addSystemComponentType<Physics::Manager>(getComponentType<Transform::Runtime_Transform>());
		//ecs_coordinator->addSystemComponentType<Physics::Manager>(getComponentType<Transform::Transform>());
		//ecs_coordinator->addSystemComponentType<Physics::Manager>(getComponentType<Move::Movement>());
		//ecs_coordinator->addSystemComponentType<Physics::Manager>(getComponentType <Collision::Collider>());

		////Register animation manager
		//ecs_coordinator->registerSystem<Animation::Manager>(Animation::Manager::getInstance());
		//accessEvents()->addEventListeners<Animation::AnimationEvent>(accessSystem<Animation::Manager>());
		//ecs_coordinator->addSystemComponentType<Animation::Manager>(getComponentType<Animation::cBase>());
		//ecs_coordinator->addSystemComponentType<Animation::Manager>(getComponentType<Animation::cSprite>());
		//ecs_coordinator->addSystemComponentType<Animation::Manager>(getComponentType<Render::Texture>());

		////Register render manager
		//ecs_coordinator->registerSystem<Render::Manager>(Render::Manager::getInstance());
		//ecs_coordinator->accessSystem<Render::Manager>()->setComponentsLinked(false);
		//ecs_coordinator->addSystemComponentType<Render::Manager>(getComponentType<Transform::Transform>());
		//ecs_coordinator->addSystemComponentType<Render::Manager>(getComponentType<Render::Shape>());
		//ecs_coordinator->addSystemComponentType<Render::Manager>(getComponentType<Render::Texture>());
		//ecs_coordinator->addSystemComponentType<Render::Manager>(getComponentType<Render::Text>());

		////Register audio manager
		//ecs_coordinator->registerSystem<Audio::Manager>(Audio::Manager::getInstance());
		//ecs_coordinator->addSystemComponentType<Audio::Manager>(getComponentType<Audio::cAudio>());
	}

	void Core::Engine::registerDefAssets() {
		//Register shader
		//assets_manager->registerShader("base", "shaders/base.vert", "shaders/base.frag");
		//assets_manager->registerShader("texture", "shaders/texture.vert", "shaders/texture.frag");

		////Register models
		//assets_manager->registerModel("square", "assets/meshes/square.txt");
		//assets_manager->registerModel("triangle", "assets/meshes/triangle.txt");
		//assets_manager->registerModel("circle", "assets/meshes/circle.txt");
		//assets_manager->registerModel("square-texture", "assets/meshes/square-texture.txt");
	}

	void Core::Engine::init(std::string const& file_path, int fps) {
		provideService(std::make_shared<Windows::Manager>());
		provideService(std::make_shared<Scenes::Manager>());
		provideService(std::make_shared<Events::Manager>());
		provideService(std::make_shared<Assets::Manager>());
		provideService(std::make_shared<Debug::Manager>());
		provideService(std::make_shared<Coordinator::Manager>());

		windows_manager = getService<Windows::Manager>();
		scenes_manager = getService<Scenes::Manager>();
		ecs_coordinator = getService<Coordinator::Manager>();

		//Create console
		#ifndef NDEBUG
		windows_manager->createConsole();
		#endif

		//Read config file
		windows_manager->readConfigFile(file_path);

		//Config glfw window system
		windows_manager->configSystem();

		//Set Target FPS
		windows_manager->setTargetFPS(fps);

		////Register Def Component
		//registerDefComponents();

		////Register Def Systems
		//registerDefSystems();

		////Register Def Assets
		//registerDefAssets();
	}

	void Core::Engine::run() {

		while (!glfwWindowShouldClose(windows_manager->getWindow())) {

			//Calculate Delta Time
			windows_manager->calculateDeltaTime();

			//Poll system events ( Interativity with app )
			glfwPollEvents();

			//Set Window Title
			windows_manager->setWinTitle(windows_manager->getWinTitle() +
				" | " + scenes_manager->getCurrSceneID() +
				" | " + std::to_string(windows_manager->getCurrentFPS()) + " fps" +
				" | " + std::to_string(ecs_coordinator->getEntitiesCount()) + " entities");

			//Update all systems
			ecs_coordinator->updateSystems();

			//Update scenes manager
			scenes_manager->update();

			//Check if window is open
			if (glfwWindowShouldClose(windows_manager->getWindow())) {

				//Terminate window
				windows_manager->terminate();
			}

			//Control FPS
			windows_manager->controlFPS();
		}
	}
}