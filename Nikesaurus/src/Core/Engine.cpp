/*****************************************************************//**
 * \file   Engine.cpp
 * \brief  Core engine arhcitecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
 //
 //Registered Systems
#include "Systems/GameLogic/sysGameLogic.h"
#include "Systems/Physics/sysPhysics.h"
#include "Systems/Animation/sysAnimation.h"
#include "Systems/sysAudio.h"
#include "Systems/Render/sysRender.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Systems/GameLogic/sysEnemy.h"

namespace NIKE {

	//Defintion of the static services member variable
	std::unordered_map<std::string, std::shared_ptr<void>> Core::Engine::services;

	void Core::Engine::registerDefComponents() {
		//Register Audio Components
		Audio::registerComponents();

		//Register transform component
		Transform::registerComponents();

		//Register physics components
		Physics::registerComponents();

		//Register animation components
		Animation::registerComponents();

		//Register render components
		Render::registerComponents();

		//Register pathfinding components
		Pathfinding::registerComponents();

		//Register render components
		GameLogic::registerComponents();

		//Register shooting components
		Shooting::registerComponents();

		//Register Enemy components
		Enemy::registerComponents();

		//Register Despawn components
		Despawn::registerComponents();

		//Register Element components
		Element::registerComponents();

		//Register Combat components
		Combat::registerComponents();
	}

	void Core::Engine::registerDefSystems() {

		//Register game logic manager
		NIKE_ECS_MANAGER->registerSystem<GameLogic::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<Shooting::Shooting>());
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<Enemy::Attack>());
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<Despawn::Lifetime>());
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<GameLogic::ILogic>());

		//Register physics manager
		NIKE_ECS_MANAGER->registerSystem<Physics::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<Physics::Manager>(NIKE_ECS_MANAGER->getComponentType<Physics::Dynamics>());
		NIKE_ECS_MANAGER->addSystemComponentType<Physics::Manager>(NIKE_ECS_MANAGER->getComponentType<Physics::Collider>());
		NIKE_ECS_MANAGER->addSystemComponentType<Physics::Manager>(NIKE_ECS_MANAGER->getComponentType<Transform::Transform>());
		NIKE_ECS_MANAGER->addSystemComponentType<Physics::Manager>(NIKE_ECS_MANAGER->getComponentType<Pathfinding::Path>());
		//game_logic_sys->registerLuaSystem(physics_sys);

		//Register animation manager
		NIKE_ECS_MANAGER->registerSystem<Animation::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<Animation::Manager>(NIKE_ECS_MANAGER->getComponentType<Animation::Base>());
		NIKE_ECS_MANAGER->addSystemComponentType<Animation::Manager>(NIKE_ECS_MANAGER->getComponentType<Animation::Sprite>());

		//Register audio system
		NIKE_ECS_MANAGER->registerSystem<Audio::Manager>();
		NIKE_ECS_MANAGER->addSystemComponentType<Audio::Manager>(NIKE_ECS_MANAGER->getComponentType<Audio::SFX>());

		////Register render manager
		//NIKE_ECS_MANAGER->registerSystem<Render::Manager>(false);
		//NIKE_ECS_MANAGER->addSystemComponentType<Render::Manager>(NIKE_ECS_MANAGER->getComponentType<Transform::Transform>());
		//NIKE_ECS_MANAGER->addSystemComponentType<Render::Manager>(NIKE_ECS_MANAGER->getComponentType<Render::Text>());
		//NIKE_ECS_MANAGER->addSystemComponentType<Render::Manager>(NIKE_ECS_MANAGER->getComponentType<Render::Shape>());
		//NIKE_ECS_MANAGER->addSystemComponentType<Render::Manager>(NIKE_ECS_MANAGER->getComponentType<Render::Texture>());

		//Register interaction manager
		auto interaction_sys = NIKE_ECS_MANAGER->registerSystem<Interaction::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<Interaction::Manager>(NIKE_ECS_MANAGER->getComponentType<Combat::Health>());
		NIKE_ECS_MANAGER->addSystemComponentType<Interaction::Manager>(NIKE_ECS_MANAGER->getComponentType<Combat::Damage>());
		NIKE_ECS_MANAGER->addSystemComponentType<Interaction::Manager>(NIKE_ECS_MANAGER->getComponentType<Physics::Collider>());
		NIKE_ECS_MANAGER->addSystemComponentType<Interaction::Manager>(NIKE_ECS_MANAGER->getComponentType<Element::Entity>());
		NIKE_ECS_MANAGER->addSystemComponentType<Interaction::Manager>(NIKE_ECS_MANAGER->getComponentType<Element::Source>());

		auto enemy_sys = NIKE_ECS_MANAGER->registerSystem<Enemy::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<Enemy::Manager>(NIKE_ECS_MANAGER->getComponentType<Transform::Transform>());
		NIKE_ECS_MANAGER->addSystemComponentType<Enemy::Manager>(NIKE_ECS_MANAGER->getComponentType<GameLogic::ILogic>());
	}

	void Core::Engine::init(std::string const& file_path, int fps, [[maybe_unused]] std::string const& custom_welcome) {		//Provide ecs coordinator service for internal engine usage
		provideService(std::make_shared<Coordinator::Manager>());

		//Provide Service
		provideService(std::make_shared<Windows::Service>());
		provideService(std::make_shared<Scenes::Service>());
		provideService(std::make_shared<Events::Service>());
		provideService(std::make_shared<Input::Service>());
		provideService(std::make_shared<Audio::Service>());
		provideService(std::make_shared<Assets::Service>());
		provideService(std::make_shared<Serialization::Service>());
		provideService(std::make_shared<Debug::Service>());
		provideService(std::make_shared<Map::Service>());
		provideService(std::make_shared<Camera::Service>());
		provideService(std::make_shared<UI::Service>());
#ifndef NDEBUG
		provideService(std::make_shared<LevelEditor::Service>());
#endif
		provideService(std::make_shared<Lua::Service>());
		provideService(std::make_shared<Path::Service>());

		//Create console
#ifndef NDEBUG
		NIKE_WINDOWS_SERVICE->createConsole(custom_welcome);
#endif

		//Init Logger
		NIKE::Log::Init();

		//Deserialize Config File
		auto json_config = NIKE_SERIALIZE_SERVICE->loadJsonFile(file_path);

		//Setup window with config file
		NIKE_WINDOWS_SERVICE->setWindow(std::make_shared<Windows::NIKEWindow>(json_config));

		//Set Target FPS
		NIKE_WINDOWS_SERVICE->setTargetFPS(fps);

		//Set up event callbacks
		NIKE_WINDOWS_SERVICE->getWindow()->setupEventCallbacks();

		//Setup input modes
		NIKE_WINDOWS_SERVICE->getWindow()->setInputMode(NIKE_CURSOR, NIKE_CURSOR_NORMAL);

		//Add event listeners for window resized
		NIKE_EVENTS_SERVICE->addEventListeners<Windows::WindowResized>(NIKE_WINDOWS_SERVICE->getWindow());
		NIKE_EVENTS_SERVICE->addEventListeners<Windows::WindowFocusEvent>(NIKE_WINDOWS_SERVICE->getWindow());

#ifndef NDEBUG
		//imgui event listeners ( ImGui listens first for all events except window resized )

		//Add event listeners for window resized
		NIKE_EVENTS_SERVICE->addEventListeners<Windows::WindowResized>(NIKE_LVLEDITOR_SERVICE);

		//Add event listeners for key event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::KeyEvent>(NIKE_LVLEDITOR_SERVICE);

		//Add event listeners for mouse event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseBtnEvent>(NIKE_LVLEDITOR_SERVICE);

		//Add event listeners for mouse move event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseMovedEvent>(NIKE_LVLEDITOR_SERVICE);

		//Add event listeners for mouse scroll event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseScrollEvent>(NIKE_LVLEDITOR_SERVICE);
#endif

		//Add event listeners for key event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::KeyEvent>(NIKE_UI_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Input::KeyEvent>(NIKE_INPUT_SERVICE);

		//Add event listeners for mouse event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseBtnEvent>(NIKE_UI_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseBtnEvent>(NIKE_INPUT_SERVICE);

		//Add event listeners for mouse move event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseMovedEvent>(NIKE_UI_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseMovedEvent>(NIKE_MAP_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseMovedEvent>(NIKE_INPUT_SERVICE);

		//Add event listeners for mouse scroll event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseScrollEvent>(NIKE_INPUT_SERVICE);

		//Init paths
		NIKE_PATH_SERVICE->init(json_config);

		//Init Audio
		NIKE_AUDIO_SERVICE->init(std::make_shared<Audio::NIKEAudioSystem>());

		//Initialize assets service
		NIKE_ASSETS_SERVICE->init(NIKE_AUDIO_SERVICE->getAudioSystem());

		//Register all assets in the game and engine folder
		NIKE_ASSETS_SERVICE->scanAssetDirectory("Game_Assets:/", true);

		//Init camera
		NIKE_CAMERA_SERVICE->init(json_config);

		//Init scene
		NIKE_SCENES_SERVICE->init();

#ifndef NDEBUG
		//Init Level Editor
		NIKE_LVLEDITOR_SERVICE->init();
#endif

		//Init UI
		NIKE_UI_SERVICE->init();

		//Init Lua
		NIKE_LUA_SERVICE->init();

		//Register Def Components
		registerDefComponents();

		//Register Def Managers
		registerDefSystems();

		int max_texture_units;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
		NIKEE_CORE_INFO("System max texture units: {0}", max_texture_units);

#ifdef NDEBUG
		// default startup to fullscreen (M3 1931)
		NIKE_WINDOWS_SERVICE->getWindow()->setFullScreen(!NIKE_WINDOWS_SERVICE->getWindow()->getFullScreen());
#endif
	}

	void Core::Engine::run() {
		// !TODO: remove this, hardcoding for installer
		NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, "physics_debug.scn"));

		while (NIKE_WINDOWS_SERVICE->getWindow()->windowState()) {
			try {

				// get delta time first
				if (NIKE_WINDOWS_SERVICE->getWindowFocus()) {
					//Calculate Delta Time
					NIKE_WINDOWS_SERVICE->calculateDeltaTime();
				}

				// have to poll events regardless of focus
				//Poll system events
				NIKE_WINDOWS_SERVICE->getWindow()->pollEvents();



					//Clear buffer
					NIKE_WINDOWS_SERVICE->getWindow()->clearBuffer();

					//Update all audio pending actions
					NIKE_AUDIO_SERVICE->getAudioSystem()->update();

					//Update scenes manager
					NIKE_SCENES_SERVICE->update();

					//Escape Key
					if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_ESCAPE)) {
						NIKE_WINDOWS_SERVICE->getWindow()->terminate();
					}

					//Toggle full screen
					if (NIKE_INPUT_SERVICE->isKeyPressed(NIKE_KEY_LEFT_CONTROL) && NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_ENTER)) {
						NIKE_WINDOWS_SERVICE->getWindow()->setFullScreen(!NIKE_WINDOWS_SERVICE->getWindow()->getFullScreen());
					}

					//Update all systems
					NIKE_ECS_MANAGER->updateSystems();

#ifndef NDEBUG
					//Update Level Editor
					NIKE_LVLEDITOR_SERVICE->update();

					//Render Level Editor
					NIKE_LVLEDITOR_SERVICE->render();

					//update UI First
					NIKE_UI_SERVICE->update();
#endif

					//Swap Buffers
					NIKE_WINDOWS_SERVICE->getWindow()->swapBuffers();

					GLenum err = glGetError();
					if (err != GL_NO_ERROR) {
						NIKEE_CORE_ERROR("OpenGL error after call to swapBuffers in {0}: {1}", __FUNCTION__, err);
					}
			}
			catch (std::runtime_error const& e) {
				NIKE_WINDOWS_SERVICE->getWindow()->setFullScreen(false);
				throw e;
			}
		}

		//Stop watching all directories
		NIKE_PATH_SERVICE->stopWatchingAllDirectories();

#ifndef NDEBUG
		//Clean up level editor
		NIKE_LVLEDITOR_SERVICE->cleanUp();
#endif

		//Clean up window resources
		NIKE_WINDOWS_SERVICE->getWindow()->cleanUp();
	}
}