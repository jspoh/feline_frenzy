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
		//Register input components
		registerComponent<Input::Key>();
		registerComponent<Input::Mouse>();

		//Register physics components
		registerComponent<Transform::Velocity>();
		registerComponent<Transform::Transform>();
		registerComponent<Transform::Runtime_Transform>();
		registerComponent<Move::Movement>();
		registerComponent<Collision::Collider>();

		//Register animation components
		registerComponent<Animation::cBase>();
		registerComponent<Animation::cSprite>();

		//Register render components
		registerComponent<Render::Shape>();
		registerComponent<Render::Texture>();
		registerComponent<Render::Color>();
		registerComponent<Render::Cam>();
		registerComponent<Render::Text>();
		registerComponent<Render::Debug>();

		//Register audio components
		registerComponent<Audio::cAudio>();
	}

	void Core::Engine::registerDefSystems() {
		//Register input manager
		registerSystem<Input::Manager>(Input::Manager::getInstance());
		accessSystem<Input::Manager>()->setComponentsLinked(false);
		addSystemComponentType<Input::Manager>(getComponentType<Input::Key>());
		addSystemComponentType<Input::Manager>(getComponentType<Input::Mouse>());
		addSystemComponentType<Input::Manager>(getComponentType<Transform::Runtime_Transform>());

		//Register physics manager
		registerSystem<Physics::Manager>(Physics::Manager::getInstance());
		accessSystem<Physics::Manager>()->setComponentsLinked(false);
		addSystemComponentType<Physics::Manager>(getComponentType<Transform::Velocity>());
		addSystemComponentType<Physics::Manager>(getComponentType<Transform::Runtime_Transform>());
		addSystemComponentType<Physics::Manager>(getComponentType<Transform::Transform>());
		addSystemComponentType<Physics::Manager>(getComponentType<Move::Movement>());
		addSystemComponentType<Physics::Manager>(getComponentType <Collision::Collider>());

		//Register animation manager
		registerSystem<Animation::Manager>(Animation::Manager::getInstance());
		accessEvents()->addEventListeners<Animation::AnimationEvent>(accessSystem<Animation::Manager>());
		addSystemComponentType<Animation::Manager>(getComponentType<Animation::cBase>());
		addSystemComponentType<Animation::Manager>(getComponentType<Animation::cSprite>());
		addSystemComponentType<Animation::Manager>(getComponentType<Render::Texture>());

		//Register render manager
		registerSystem<Render::Manager>(Render::Manager::getInstance());
		accessSystem<Render::Manager>()->setComponentsLinked(false);
		addSystemComponentType<Render::Manager>(getComponentType<Transform::Transform>());
		addSystemComponentType<Render::Manager>(getComponentType<Render::Shape>());
		addSystemComponentType<Render::Manager>(getComponentType<Render::Texture>());
		addSystemComponentType<Render::Manager>(getComponentType<Render::Text>());

		//Register audio manager
		registerSystem<Audio::Manager>(Audio::Manager::getInstance());
		addSystemComponentType<Audio::Manager>(getComponentType<Audio::cAudio>());
	}

	void Core::Engine::registerDefAssets() {
		//Register shader
		assets_manager->registerShader("base", "shaders/base.vert", "shaders/base.frag");
		assets_manager->registerShader("texture", "shaders/texture.vert", "shaders/texture.frag");

		////Register models
		//assets_manager->registerModel("square", "assets/meshes/square.txt");
		//assets_manager->registerModel("triangle", "assets/meshes/triangle.txt");
		//assets_manager->registerModel("circle", "assets/meshes/circle.txt");
		//assets_manager->registerModel("square-texture", "assets/meshes/square-texture.txt");
	}

	void Core::Engine::init(std::string const& file_path, int fps) {
		entity_manager = std::make_unique<Entity::Manager>();
		component_manager = std::make_unique<Component::Manager>();
		system_manager = std::make_unique<System::Manager>();
		windows_manager = std::make_unique<Windows::Manager>();
		scenes_manager = std::make_unique<Scenes::Manager>();
		events_manager = std::make_unique<Events::Manager>();
		assets_manager = std::make_unique<Assets::Manager>();
		debug_manager = std::make_unique<Debug::Manager>();
		seri_manager = std::make_unique<Serialization::Manager>();

		//Read config file
		windows_manager->readConfigFile(file_path);

		//Config glfw window system
		windows_manager->configSystem();

		//Set Target FPS
		windows_manager->setTargetFPS(fps);

		//Register Def Component
		registerDefComponents();

		//Register Def Systems
		registerDefSystems();

		//Register Def Assets
		registerDefAssets();
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
				" | " + std::to_string(entity_manager->getEntityCount()) + " entities");

			//Update all systems
			system_manager->updateSystems();

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

	/*****************************************************************//**
	* Access Windows
	*********************************************************************/
	std::unique_ptr<Windows::Manager>& Core::Engine::accessWindow() {
		return windows_manager;
	}

	/*****************************************************************//**
	* Access Scenes
	*********************************************************************/
	std::unique_ptr<Scenes::Manager>& Core::Engine::accessScenes() {
		return scenes_manager;
	}

	/*****************************************************************//**
	* Access Events
	*********************************************************************/
	std::unique_ptr<Events::Manager>& Core::Engine::accessEvents() {
		return events_manager;
	}

	/*****************************************************************//**
	* Access Assets
	*********************************************************************/
	std::unique_ptr<Assets::Manager>& Core::Engine::accessAssets() {
		return assets_manager;
	}

	/*****************************************************************//**
	* Access Debug
	*********************************************************************/

	std::unique_ptr<Debug::Manager>& Core::Engine::accessDebug()
	{
		return debug_manager;
	}

	/*****************************************************************//**
	* Access Serialization
	*********************************************************************/

	std::unique_ptr<Serialization::Manager>& Core::Engine::accessSeri()
	{
		return seri_manager;
	}
}