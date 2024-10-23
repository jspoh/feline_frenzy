/*****************************************************************//**
 * \file   Engine.cpp
 * \brief  Core engine arhcitecture
 * 
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"

////Registerd Components
#include "Components/cAudio.h"
//#include "../headers/Components/cMove.h"
// //#include "../headers/Components/cRender.h"
//
//Registered Systems
#include "Systems/sysAudio.h"
#include "Systems/sysImgui.h"
//#include "../headers/Systems/sysInput.h"
//#include "../headers/Systems/Physics/sysPhysics.h"
//#include "../headers/Systems/Animation/sysAnimation.h"
//#include "../headers/Systems/Render/sysRender.h"
//#include "../headers/Systems/GameLogic/sysObjectSpawner.h"

namespace NIKESAURUS {

	//Defintion of the static services member variable
	std::unordered_map<std::string, std::shared_ptr<void>> NIKESAURUS::Core::Engine::services;

	Core::Engine::~Engine() {
		glfwTerminate();
	}

	void Core::Engine::registerDefComponents() {
		//Register Audio Components
		getService<Coordinator::Manager>()->registerComponent<Audio::SFX>();

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
		////Register audio system
		getService<Coordinator::Manager>()->registerSystem<Audio::Manager>();
		getService<Coordinator::Manager>()->addSystemComponentType<Audio::Manager>(getService<Coordinator::Manager>()->getComponentType<Audio::SFX>());

		////Register input manager
		//ecs_coordinator->registerSystem<Input::Manager>(Input::Manager::getInstance());
		//ecs_coordinator->accessSystem<Input::Manager>()->setComponentsLinked(false);
		//ecs_coordinator->addSystemComponentType<Input::Manager>(getComponentType<Input::Key>());
		//ecs_coordinator->addSystemComponentType<Input::Manager>(getComponentType<Input::Mouse>());
		//ecs_coordinator->addSystemComponentType<Input::Manager>(getComponentType<Transform::Runtime_Transform>());

		// Regiser imgui manager here, After input before render
		getService<Coordinator::Manager>()->registerSystem<IMGUI::Manager>();

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

	void Core::Engine::init(std::string const& file_path, int fps, [[maybe_unused]] std::string const& custom_welcome) {

		//Provide Services
		provideService(std::make_shared<Windows::Service>());
		provideService(std::make_shared<Scenes::Service>());
		provideService(std::make_shared<Events::Service>());
		provideService(std::make_shared<Input::Service>());
		provideService(std::make_shared<Audio::Service>());
		provideService(std::make_shared<Assets::Service>());
		provideService(std::make_shared<Debug::Service>());
		provideService(std::make_shared<Coordinator::Manager>());

		//Create console
		#ifndef NDEBUG
		getService<Windows::Service>()->createConsole(custom_welcome);
		#endif

		//Init Logger
		NIKESAURUS::Log::Init();

		//Setup window with config file
		getService<Windows::Service>()->setWindow(std::make_shared<Windows::NIKEWindow>(file_path));

		//Set Target FPS
		getService<Windows::Service>()->setTargetFPS(fps);

		//Set up event callbacks
		getService<Windows::Service>()->getWindow()->setupEventCallbacks();

		//Setup input modes
		NIKEEngine.getService<Windows::Service>()->getWindow()->setInputMode(NIKE_CURSOR, NIKE_CURSOR_NORMAL);

		//Add Event Listeners
		getService<Events::Service>()->addEventListeners<Windows::WindowResized>(NIKEEngine.getService<Windows::Service>()->getWindow());
		getService<Events::Service>()->addEventListeners<Input::KeyEvent>(NIKEEngine.getService<Input::Service>());
		getService<Events::Service>()->addEventListeners<Input::MouseBtnEvent>(NIKEEngine.getService<Input::Service>());
		getService<Events::Service>()->addEventListeners<Input::MouseMovedEvent>(NIKEEngine.getService<Input::Service>());
		getService<Events::Service>()->addEventListeners<Input::MouseScrollEvent>(NIKEEngine.getService<Input::Service>());

		//Setup Audio
		getService<Audio::Service>()->setAudioSystem(std::make_shared<Audio::NIKEAudioSystem>());

		//Setup assets loading with systems for loading
		getService<Assets::Service>()->configAssets(getService<Audio::Service>()->getAudioSystem());

		//Register Def Components
		registerDefComponents();
		
		//Register Def Managers
		registerDefSystems();
	}

	void Core::Engine::run() {

		while (getService<Windows::Service>()->getWindow()->windowState()) {

			//Calculate Delta Time
			getService<Windows::Service>()->calculateDeltaTime();

			//Set Window Title
			getService<Windows::Service>()->getWindow()->setWindowTitle(getService<Windows::Service>()->getWindow()->getWindowTitle() +
				" | " + getService<Scenes::Service>()->getCurrSceneID() +
				" | " + std::to_string(getService<Windows::Service>()->getCurrentFPS()) + " fps" +
				" | " + std::to_string(getService<Coordinator::Manager>()->getEntitiesCount()) + " entities");

			//Poll system events
			getService<Windows::Service>()->getWindow()->pollEvents();

			//Update all audio pending actions
			getService<Audio::Service>()->getAudioSystem()->update();

			//Update all systems
			getService<Coordinator::Manager>()->updateSystems();

			//Update scenes manager
			getService<Scenes::Service>()->update();

			//Escape Key Testing //!MOVE OUT SOON
			if (getService<Input::Service>()->isKeyTriggered(NIKE_KEY_ESCAPE)) {
				getService<Windows::Service>()->getWindow()->terminate();
			}

			//Audio Testing //!MOVE OUT SOON
			if (getService<Input::Service>()->isKeyTriggered(NIKE_KEY_ENTER)) {
				getService<Audio::Service>()->playAudio("SFX", "", "MASTER", 1.0f, 1.0f, false);
			}

			//Control FPS
			getService<Windows::Service>()->controlFPS();
		}


		//Clean up window resources
		getService<Windows::Service>()->getWindow()->cleanUp();
	}
}