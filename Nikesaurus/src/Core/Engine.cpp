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

		// Regiser state component
		State::registerComponents();
	}

	void Core::Engine::registerDefSystems() {

		//Register game logic manager
		NIKE_ECS_MANAGER->registerSystem<GameLogic::Manager>(false);
		NIKE_ECS_MANAGER->addSystemComponentType<GameLogic::Manager>(NIKE_ECS_MANAGER->getComponentType<Shooting::Shooting>());
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
		//game_logic_sys->registerLuaSystem(physics_sys);

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

		//Setup input modes
		NIKE_WINDOWS_SERVICE->getWindow()->setInputMode(NIKE_CURSOR, NIKE_CURSOR_NORMAL);

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


		//Add event listeners for mouse move event
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseMovedEvent>(NIKE_INPUT_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseMovedEvent>(NIKE_UI_SERVICE);
		NIKE_EVENTS_SERVICE->addEventListeners<Input::MouseMovedEvent>(NIKE_MAP_SERVICE);

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

		//Init UI
		NIKE_UI_SERVICE->init();

		//Init Lua
		NIKE_LUA_SERVICE->init();

#ifndef NDEBUG
		//Init Level Editor
		NIKE_LVLEDITOR_SERVICE->init();
#endif

		// Init FSM
		NIKE_FSM_SERVICE->init();

		//Register Def Components
		registerDefComponents();

		//Register Def Managers
		registerDefSystems();

#ifndef NDEBUG
		NIKE_LVLEDITOR_SERVICE->deserializeConfig(json_config);
#endif

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
		NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, "lvl1.scn"));

		//NIKE::Render::Manager::addEntity();
		//constexpr const char* FPS_DISPLAY_NAME = "FPS Display";
		//Entity::Type FPS_DISPLAY_ENTITY = NIKE_ECS_MANAGER->createEntity(0);
		//NIKE_ECS_MANAGER->addEntityComponent<Transform::Transform>(FPS_DISPLAY_ENTITY, Transform::Transform({ 790.f, 420.f }, { 600.f, 150.f }, 0.f, true));
		//NIKE_ECS_MANAGER->addEntityComponent<Render::Text>(FPS_DISPLAY_ENTITY, Render::Text("Skranji-Bold.ttf", "20000000 FPS", {1.f, 1.f, 1.f, 1.f}, 1.0f));

		std::stringstream ss;
		Entity::Type FPS_DISPLAY_ENTITY{};
		int frame_count = 0;
		std::vector<float> fps_history;
		fps_history.reserve(300);		// unlikely to exceed 300fps
		float elapsed_time = 0.f;
		while (NIKE_WINDOWS_SERVICE->getWindow()->windowState()) {
			try {

				// have to poll events regardless of focus
				//Poll system events
				NIKE_WINDOWS_SERVICE->getWindow()->pollEvents();

				if (!NIKE_WINDOWS_SERVICE->getWindowFocus()) {
					// Skip updates when unfocused, but continue polling events
					NIKE_WINDOWS_SERVICE->getWindow()->clearBuffer();

					continue;
				}
				//Calculate Delta Time
				NIKE_WINDOWS_SERVICE->calculateDeltaTime();

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
				//Update map grid
				NIKE_MAP_SERVICE->gridUpdate();

				//Swap Buffers
				NIKE_WINDOWS_SERVICE->getWindow()->swapBuffers();

				GLenum err = glGetError();
				if (err != GL_NO_ERROR) {
					NIKEE_CORE_ERROR("OpenGL error after call to swapBuffers in {0}: {1}", __FUNCTION__, err);
				}


				// update rendered fps
				static std::unordered_map<std::string, std::shared_ptr<void>> comps;
				static Render::Text* comp = nullptr;

				elapsed_time += NIKE_WINDOWS_SERVICE->getDeltaTime();

				comps = NIKE_ECS_MANAGER->getAllEntityComponents(FPS_DISPLAY_ENTITY);
				comp = reinterpret_cast<Render::Text*>(comps["Render::Text"].get());
				if (comp) {
					// toggle fps display
					if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_F1)) {
						if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Hidden>(FPS_DISPLAY_ENTITY)) {
							NIKE_ECS_MANAGER->removeEntityComponent<Render::Hidden>(FPS_DISPLAY_ENTITY);
						}
						else {
							NIKE_ECS_MANAGER->addEntityComponent<Render::Hidden>(FPS_DISPLAY_ENTITY, {});
						}
					}

					// calculatee fps
					fps_history.push_back(NIKE_WINDOWS_SERVICE->getCurrentFPS());
					if (elapsed_time < 1.f) {		// only update fps with average every n second
						continue;			// CONTINUE CALL HERE. SO IF ANYTHING IS ADDED AFTER THIS, THIS MUST BE EDITED
					}

					elapsed_time = 0.f;
					float sum_fps = 0;
					std::for_each(fps_history.begin(), fps_history.end(), [&sum_fps](float& fps) { sum_fps += fps; });
					const float avg_fps = sum_fps / fps_history.size();
					fps_history.clear();

					// update fps text
					comps = NIKE_ECS_MANAGER->getAllEntityComponents(FPS_DISPLAY_ENTITY);
					comp = reinterpret_cast<Render::Text*>(comps["Render::Text"].get());
					ss << "FPS: " << std::round(avg_fps);
					comp->text = ss.str();
					ss.str("");
					ss.clear();
				}
				else {
					// initialization of fps text
					constexpr const char* FPS_DISPLAY_NAME = "FPS Display";
					FPS_DISPLAY_ENTITY = NIKE_ECS_MANAGER->createEntity(0);

#ifndef NDEBUG
					std::shared_ptr<LevelEditor::EntitiesPanel> entities_panel = std::dynamic_pointer_cast<LevelEditor::EntitiesPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(NIKE::LevelEditor::EntitiesPanel::getStaticName()));
					if (!entities_panel) {
						NIKEE_CORE_ERROR("Entities Panel not found");
						throw;
					}
					auto& entity_map = entities_panel->getEntityMap();
					auto& entity_to_name_map = entities_panel->getEntityToNameMap();
					auto& name_to_entity_map = entities_panel->getNameToEntityMap();

					entity_map[FPS_DISPLAY_ENTITY] = LevelEditor::EntityMetaData(FPS_DISPLAY_NAME, "", false);
					name_to_entity_map.erase(entity_to_name_map[FPS_DISPLAY_ENTITY]);
					name_to_entity_map[FPS_DISPLAY_NAME] = FPS_DISPLAY_ENTITY;
					entity_to_name_map[FPS_DISPLAY_ENTITY] = FPS_DISPLAY_NAME;
#endif

					NIKE_ECS_MANAGER->addEntityComponent<Transform::Transform>(FPS_DISPLAY_ENTITY, Transform::Transform({ 600.f, 420.f }, { 600.f, 150.f }, 0.f, true));
					NIKE_ECS_MANAGER->addEntityComponent<Render::Text>(FPS_DISPLAY_ENTITY, Render::Text("Skranji-Bold.ttf", "FPS:", { 1.f, 1.f, 1.f, 1.f }, 1.0f));
					NIKE_ECS_MANAGER->addEntityComponent<Render::BuiltIn>(FPS_DISPLAY_ENTITY, { });

					comps = NIKE_ECS_MANAGER->getAllEntityComponents(FPS_DISPLAY_ENTITY);
					comp = reinterpret_cast<Render::Text*>(comps["Render::Text"].get());
					comp->origin = Render::TextOrigin::LEFT;
				}

				frame_count++;
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