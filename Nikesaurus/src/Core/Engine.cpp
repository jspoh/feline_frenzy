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
#include "Managers/Services/State Machine/enemyUtils.h"

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

		//Register render components
		GameLogic::registerComponents();

		//Register Enemy components
		Enemy::registerComponents();

		//Register Despawn components
		Despawn::registerComponents();

		//Register Element components
		Element::registerComponents();

		//Register Combat components
		Combat::registerComponents();

		// Regiser state component
		State::registerComponents();
	}

	void Core::Engine::registerDefEditorComponents() {
		//Register Audio Components
		Audio::registerEditorComponents();

		//Register transform component
		Transform::registerEditorComponents();

		//Register physics components
		Physics::registerEditorComponents();

		//Register animation components
		Animation::registerEditorComponents();

		//Register render components
		Render::registerEditorComponents();

		//Register render components
		GameLogic::registerEditorComponents();

		//Register Enemy components
		Enemy::registerEditorComponents();

		//Register Despawn components
		Despawn::registerEditorComponents();

		//Register Element components
		Element::registerEditorComponents();

		//Register Combat components
		Combat::registerEditorComponents();

		// Regiser state component
		State::registerEditorComponents();
	}

	void Core::Engine::registerDefSystems() {

		//Register game logic manager
		NIKE_ECS_MANAGER->registerSystem<GameLogic::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<Enemy::Attack>());
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<Despawn::Lifetime>());
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<GameLogic::ILogic>());
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<State::State>());
		// Yes i know it shldnt be here, but we make do first
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<Transform::Transform>());
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<Physics::Dynamics>());

		//Register physics manager
		NIKE_ECS_MANAGER->registerSystem<Physics::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<Physics::Manager>(NIKE_ECS_MANAGER->getComponentType<Physics::Dynamics>());
		NIKE_ECS_MANAGER->addSystemComponentType<Physics::Manager>(NIKE_ECS_MANAGER->getComponentType<Physics::Collider>());
		NIKE_ECS_MANAGER->addSystemComponentType<Physics::Manager>(NIKE_ECS_MANAGER->getComponentType<Transform::Transform>());

		//Register animation manager
		NIKE_ECS_MANAGER->registerSystem<Animation::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<Animation::Manager>(NIKE_ECS_MANAGER->getComponentType<Animation::Base>());
		NIKE_ECS_MANAGER->addSystemComponentType<Animation::Manager>(NIKE_ECS_MANAGER->getComponentType<Animation::Sprite>());

		//Register audio system
		NIKE_ECS_MANAGER->registerSystem<Audio::Manager>();
		NIKE_ECS_MANAGER->addSystemComponentType<Audio::Manager>(NIKE_ECS_MANAGER->getComponentType<Audio::SFX>());

		//Register render manager
		NIKE_ECS_MANAGER->registerSystem<Render::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<Render::Manager>(NIKE_ECS_MANAGER->getComponentType<Transform::Transform>());
		NIKE_ECS_MANAGER->addSystemComponentType<Render::Manager>(NIKE_ECS_MANAGER->getComponentType<Render::Text>());
		NIKE_ECS_MANAGER->addSystemComponentType<Render::Manager>(NIKE_ECS_MANAGER->getComponentType<Render::Shape>());
		NIKE_ECS_MANAGER->addSystemComponentType<Render::Manager>(NIKE_ECS_MANAGER->getComponentType<Render::Texture>());

		//Register interaction manager
		auto interaction_sys = NIKE_ECS_MANAGER->registerSystem<Interaction::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<Interaction::Manager>(NIKE_ECS_MANAGER->getComponentType<Combat::Health>());
		NIKE_ECS_MANAGER->addSystemComponentType<Interaction::Manager>(NIKE_ECS_MANAGER->getComponentType<Combat::Damage>());
		NIKE_ECS_MANAGER->addSystemComponentType<Interaction::Manager>(NIKE_ECS_MANAGER->getComponentType<Physics::Collider>());
		NIKE_ECS_MANAGER->addSystemComponentType<Interaction::Manager>(NIKE_ECS_MANAGER->getComponentType<Element::Entity>());
		NIKE_ECS_MANAGER->addSystemComponentType<Interaction::Manager>(NIKE_ECS_MANAGER->getComponentType<Element::Source>());

		//auto enemy_sys = NIKE_ECS_MANAGER->registerSystem<Enemy::Manager>(false);
		//NIKE_ECS_MANAGER->addSystemComponentType<Enemy::Manager>(NIKE_ECS_MANAGER->getComponentType<Transform::Transform>());
		//NIKE_ECS_MANAGER->addSystemComponentType<Enemy::Manager>(NIKE_ECS_MANAGER->getComponentType<GameLogic::ILogic>());
		//NIKE_ECS_MANAGER->addSystemComponentType<Enemy::Manager>(NIKE_ECS_MANAGER->getComponentType<Pathfinding::Path>());
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
		provideService(std::make_shared<Render::Service>());
		provideService(std::make_shared<Debug::Service>());
		provideService(std::make_shared<Map::Service>());
		provideService(std::make_shared<Camera::Service>());
		provideService(std::make_shared<UI::Service>());
#ifndef NDEBUG
		provideService(std::make_shared<LevelEditor::Service>());
#endif
		provideService(std::make_shared<Lua::Service>());
		provideService(std::make_shared<Path::Service>());
		provideService(std::make_shared<MetaData::Service>());
		provideService(std::make_shared<StateMachine::Service>());

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

		//Add event listeners for window resized
		NIKE_EVENTS_SERVICE->addEventListeners<Windows::WindowResized>(NIKE_WINDOWS_SERVICE->getWindow());
		NIKE_EVENTS_SERVICE->addEventListeners<Windows::WindowFocusEvent>(NIKE_WINDOWS_SERVICE->getWindow());

#ifndef NDEBUG
		//imgui event listeners ( ImGui listens first for all events except window resized )

		//Add event listeners for window resized
		NIKE_EVENTS_SERVICE->addEventListeners<Windows::WindowResized>(NIKE_LVLEDITOR_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Windows::WindowResized>(NIKE_RENDER_SERVICE);
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
		NIKE_EVENTS_SERVICE->addEventListeners<Input::KeyEvent>(NIKE_INPUT_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Input::KeyEvent>(NIKE_UI_SERVICE);


		//Add event listeners for mouse event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseBtnEvent>(NIKE_INPUT_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseBtnEvent>(NIKE_UI_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Input::CursorEnterEvent>(NIKE_INPUT_SERVICE);


		//Add event listeners for mouse move event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseMovedEvent>(NIKE_INPUT_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseMovedEvent>(NIKE_UI_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseMovedEvent>(NIKE_MAP_SERVICE);

		//Add event listeners for mouse scroll event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseScrollEvent>(NIKE_INPUT_SERVICE);

		//Init paths
		NIKE_PATH_SERVICE->init(json_config);

		//Init Audio
		NIKE_AUDIO_SERVICE->init(json_config);

		//Initialize assets service
		NIKE_ASSETS_SERVICE->init(NIKE_AUDIO_SERVICE->getAudioSystem());

		//Register all assets in the game and engine folder
		NIKE_ASSETS_SERVICE->scanAssetDirectory("Game_Assets:/", true);

		//Init camera
		NIKE_CAMERA_SERVICE->init(json_config);

		//Init scene
		NIKE_SCENES_SERVICE->init();

		//Init UI
		NIKE_UI_SERVICE->init();

		//Init Lua
		NIKE_LUA_SERVICE->init();

		//Init metadata service
		NIKE_METADATA_SERVICE->init();

		NIKE_FSM_SERVICE->init();

		//Register Def Components
		registerDefComponents();

		//Register Def Managers
		registerDefSystems();

#ifndef NDEBUG
		//Init Level Editor
		NIKE_LVLEDITOR_SERVICE->init();
		NIKE_LVLEDITOR_SERVICE->deserializeConfig(json_config);

		//Register default editor components
		registerDefEditorComponents();
#endif

		int max_texture_units;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
		NIKEE_CORE_INFO("System max texture units: {0}", max_texture_units);

#ifdef NDEBUG
		// default startup to fullscreen (M3 1931)
		NIKE_WINDOWS_SERVICE->getWindow()->setFullScreen(!NIKE_WINDOWS_SERVICE->getWindow()->getFullScreen());
#endif

#ifndef NDEBUG
		NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, "main_menu.scn"));
#endif 
#ifdef NDEBUG
		NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, "main_menu.scn"));
#endif
	}

	void Core::Engine::updateLogic() {

		//Calculate Delta Time
		NIKE_WINDOWS_SERVICE->calculateDeltaTime();

		//Update all systems ( Always update systems before any other services )
		NIKE_ECS_MANAGER->updateSystems();

		//Update meta data
		NIKE_METADATA_SERVICE->update();

		//Update scenes manager
		NIKE_SCENES_SERVICE->update();

		//Update map grid
		NIKE_MAP_SERVICE->gridUpdate();

		//Update all audio pending actions
		NIKE_AUDIO_SERVICE->getAudioSystem()->update();

#ifndef NDEBUG
		//Update & Render Level Editor
		NIKE_LVLEDITOR_SERVICE->updateAndRender();
#endif

		//update UI First
		NIKE_UI_SERVICE->update();

		//Update input service
		NIKE_INPUT_SERVICE->update();

		//Swap Buffers
		NIKE_WINDOWS_SERVICE->getWindow()->swapBuffers();

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error after call to swapBuffers in {0}: {1}", __FUNCTION__, err);
		}
	}

	void Core::Engine::run() {

		//Update loop
		while (NIKE_WINDOWS_SERVICE->getWindow()->windowState()) {

			// have to poll events regardless of focus
			//Poll system events
			NIKE_WINDOWS_SERVICE->getWindow()->pollEvents();

			//Skip update when window is out of focus
			if (!NIKE_WINDOWS_SERVICE->getWindowFocus()) {
				continue;
			}

			//Escape Key
			//if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_ESCAPE)) {
			//	if (NIKE_WINDOWS_SERVICE->getWindow()->getFullScreen()) {
			//		NIKE_WINDOWS_SERVICE->getWindow()->setFullScreen(false);
			//	}
			//	else {
			//		NIKE_WINDOWS_SERVICE->getWindow()->terminate();
			//	}
			//}

			//Toggle full screen
			if (NIKE_INPUT_SERVICE->isKeyPressed(NIKE_KEY_LEFT_CONTROL) && NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_ENTER)) {
				NIKE_WINDOWS_SERVICE->getWindow()->setFullScreen(!NIKE_WINDOWS_SERVICE->getWindow()->getFullScreen());
			}

			//Check if game is full screened
			if (NIKE_WINDOWS_SERVICE->getWindow()->getFullScreen()) {
				try {
					//Implement update logic
					updateLogic();
				}
				catch (std::runtime_error const& e) {
					NIKE_WINDOWS_SERVICE->getWindow()->setFullScreen(false);
					NIKEE_CORE_ERROR("Error caught: {}", e.what());
					break;
				}
			}
			else {
				//Implement update logic
				updateLogic();
			}
		}

		//Stop watching all directories
		NIKE_PATH_SERVICE->stopWatchingAllDirectories();

#ifndef NDEBUG
		//Auto Save if needed
		NIKE_LVLEDITOR_SERVICE->autoSave();

		//Clean up level editor
		NIKE_LVLEDITOR_SERVICE->cleanUp();
#endif

		//Clean up window resources
		NIKE_WINDOWS_SERVICE->getWindow()->cleanUp();
	}
}