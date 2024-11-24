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
	}

	void Core::Engine::registerDefSystems() {

		//Register game logic manager
		auto game_logic_sys = NIKE_ECS_MANAGER->registerSystem<GameLogic::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<GameLogic::Movement>());

		//Register physics manager
		auto physics_sys = NIKE_ECS_MANAGER->registerSystem<Physics::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<Physics::Manager>(NIKE_ECS_MANAGER->getComponentType<Physics::Dynamics>());
		NIKE_ECS_MANAGER->addSystemComponentType<Physics::Manager>(NIKE_ECS_MANAGER->getComponentType<Physics::Collider>());
		NIKE_ECS_MANAGER->addSystemComponentType<Physics::Manager>(NIKE_ECS_MANAGER->getComponentType<Transform::Transform>());
		NIKE_ECS_MANAGER->addSystemComponentType<Physics::Manager>(NIKE_ECS_MANAGER->getComponentType<Pathfinding::Path>());
		game_logic_sys->registerLuaSystem(physics_sys);

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
	}

	void Core::Engine::init(std::string const& file_path, int fps, [[maybe_unused]] std::string const& custom_welcome) {

		//Provide ecs coordinator service for internal engine usage
		provideService(std::make_shared<Coordinator::Manager>());

		//Provide Services
		provideService(std::make_shared<Windows::Service>());
		provideService(std::make_shared<Scenes::Service>());
		provideService(std::make_shared<Events::Service>());
		provideService(std::make_shared<Input::Service>());
		provideService(std::make_shared<Audio::Service>());
		provideService(std::make_shared<Assets::Service>());
		provideService(std::make_shared<Serialization::Service>());
		provideService(std::make_shared<Debug::Service>());
		provideService(std::make_shared<IMGUI::Service>());
		provideService(std::make_shared<Map::Service>());
		provideService(std::make_shared<Camera::Service>());
		provideService(std::make_shared<UI::Service>());
		provideService(std::make_shared<LevelEditor::Service>());
		provideService(std::make_shared<Lua::Service>());

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
		getService<Events::Service>()->addEventListeners<Windows::WindowResized>(NIKE_WINDOWS_SERVICE->getWindow());
		getService<Events::Service>()->addEventListeners<Windows::WindowFocusEvent>(NIKE_WINDOWS_SERVICE->getWindow());
		getService<Events::Service>()->addEventListeners<Windows::WindowResized>(NIKE_LVLEDITOR_SERVICE);

		//Add event listeners for key event
		getService<Events::Service>()->addEventListeners<Input::KeyEvent>(NIKE_INPUT_SERVICE);
		getService<Events::Service>()->addEventListeners<Input::KeyEvent>(NIKE_LVLEDITOR_SERVICE);
		getService<Events::Service>()->addEventListeners<Input::KeyEvent>(NIKE_UI_SERVICE);

		//Add event listeners for mouse event
		getService<Events::Service>()->addEventListeners<Input::MouseBtnEvent>(NIKE_INPUT_SERVICE);
		getService<Events::Service>()->addEventListeners<Input::MouseBtnEvent>(NIKE_LVLEDITOR_SERVICE);
		getService<Events::Service>()->addEventListeners<Input::MouseBtnEvent>(NIKE_UI_SERVICE);

		//Add event listeners for mouse move event
		getService<Events::Service>()->addEventListeners<Input::MouseMovedEvent>(NIKE_INPUT_SERVICE);
		getService<Events::Service>()->addEventListeners<Input::MouseMovedEvent>(NIKE_LVLEDITOR_SERVICE);
		getService<Events::Service>()->addEventListeners<Input::MouseMovedEvent>(NIKE_MAP_SERVICE);
		getService<Events::Service>()->addEventListeners<Input::MouseMovedEvent>(NIKE_UI_SERVICE);

		//Add event listeners for mouse scroll event
		getService<Events::Service>()->addEventListeners<Input::MouseScrollEvent>(NIKE_INPUT_SERVICE);
		getService<Events::Service>()->addEventListeners<Input::MouseScrollEvent>(NIKE_LVLEDITOR_SERVICE);

		//Add event listeners for drop files event
		getService<Events::Service>()->addEventListeners<Assets::FileDropEvent>(NIKE_ASSETS_SERVICE);

		//Setup Audio
		getService<Audio::Service>()->setAudioSystem(std::make_shared<Audio::NIKEAudioSystem>());

		//Setup assets loading with systems for loading
		getService<Assets::Service>()->configAssets(getService<Audio::Service>()->getAudioSystem());

		//Init imgui
		//NIKE_IMGUI_SERVICE->init();

		//Init camera
		NIKE_CAMERA_SERVICE->init(json_config);

		//Init Level Editor
		NIKE_LVLEDITOR_SERVICE->init();

		//Init UI
		NIKE_UI_SERVICE->init();

		//Register Def Components
		registerDefComponents();

		//Register Def Managers
		registerDefSystems();

		int max_texture_units;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
		NIKEE_CORE_INFO("System max texture units: {0}", max_texture_units);
	}

	void Core::Engine::run() {

		while (NIKE_WINDOWS_SERVICE->getWindow()->windowState()) {

			//Calculate Delta Time
			NIKE_WINDOWS_SERVICE->calculateDeltaTime();

			//Poll system events
			NIKE_WINDOWS_SERVICE->getWindow()->pollEvents();

			//Clear buffer
			NIKE_WINDOWS_SERVICE->getWindow()->clearBuffer();

			//Update all audio pending actions
			NIKE_AUDIO_SERVICE->getAudioSystem()->update();

			//update UI First
			NIKE_UI_SERVICE->update();

			//Update scenes manager
			NIKE_SCENES_SERVICE->update();

			static constexpr bool JS_TEXTURE_TEST = true;
			//Render entity to mouse click
			if 
				(JS_TEXTURE_TEST && NIKE_INPUT_SERVICE->isMousePressed(NIKE_MOUSE_BUTTON_LEFT)) {

				static constexpr int NUM_ENTITIES_TO_SPAWN = 1;

				for (int _{}; _ < NUM_ENTITIES_TO_SPAWN; _++) {
					Entity::Type entity = NIKE_ECS_MANAGER->createEntity();
					Vector2f randsize{ Utility::randFloat() * 50.0f, Utility::randFloat() * 50.0f };
					Vector2f randpos{ NIKE_INPUT_SERVICE->getMousePos().x - (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x / 2.0f), -(NIKE_INPUT_SERVICE->getMousePos().y - (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y / 2.0f)) };
					NIKE_ECS_MANAGER->addEntityComponent<Transform::Transform>(entity, Transform::Transform(randpos, randsize, Utility::randFloat() * 360.0f));
					NIKE_ECS_MANAGER->addEntityComponent<Render::Shape>(entity, Render::Shape("square", { Utility::randFloat() ,Utility::randFloat() , Utility::randFloat() , 1.f }));
					NIKE_ECS_MANAGER->addEntityComponent<Render::Texture>(entity, Render::Texture("Tree_Orange", { 1.0f, 1.0f, 1.0f, 1.0f }));
				}
			}

			//Escape Key
			if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_ESCAPE)) {
				NIKE_WINDOWS_SERVICE->getWindow()->terminate();
			}

			//Toggle full screen
			if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_ENTER)) {
				NIKE_WINDOWS_SERVICE->getWindow()->setFullScreen(!NIKE_WINDOWS_SERVICE->getWindow()->getFullScreen());
			}

			//Update all systems
			NIKE_ECS_MANAGER->updateSystems();

			////ImGui Render & Update
			//NIKE_IMGUI_SERVICE->update();

			//Update Level Editor
			NIKE_LVLEDITOR_SERVICE->update();

			//Render Level Editor
			NIKE_LVLEDITOR_SERVICE->render();

			//Swap Buffers
			NIKE_WINDOWS_SERVICE->getWindow()->swapBuffers();
		}

		//Clean up level editor
		NIKE_LVLEDITOR_SERVICE->cleanUp();

		//Clean up window resources
		NIKE_WINDOWS_SERVICE->getWindow()->cleanUp();
	}
}