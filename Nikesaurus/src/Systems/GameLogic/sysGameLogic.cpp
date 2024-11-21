/*****************************************************************//**
 * \file   sysGameLogic.cpp
 * \brief  game logic system
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (50%)
 * \co-author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (50%)
 * \date   September 2024
 *********************************************************************/

#include "Core/stdafx.h"
#include "Systems/GameLogic/sysGameLogic.h"
#include "Core/Engine.h"
//#include "Systems/GameLogic/sysLua.h"

namespace NIKE {

	void GameLogic::Manager::init() {
		lua_system = std::make_unique<Lua::System>();
		lua_system->init();
	}

	void GameLogic::Manager::registerLuaSystem(std::shared_ptr<Lua::ILuaBind> system) {
		//Add system to lua
		lua_system->registerLuaSystem(system);
	}

	sol::protected_function GameLogic::Manager::executeScript(std::string const& file_path, std::string& script_id, bool& b_loaded, std::string const& function) {
		//Run script
		if (script_id == "") {
			script_id = lua_system->loadScript(file_path);
			b_loaded = true;
			return lua_system->executeScript(script_id, function);
		}
		else if (b_loaded) {
			return lua_system->executeScript(script_id, function);
		}
		else {
			lua_system->reloadScript(script_id);
			b_loaded = true;
			return lua_system->executeScript(script_id, function);
		}
	}

	void GameLogic::Manager::update() {
		//Get layers
		auto& layers = NIKE_SCENES_SERVICE->getCurrScene()->getLayers();

		//Reverse Iterate through layers
		for (auto layer = layers.rbegin(); layer != layers.rend(); layer++) {

			//SKip inactive layer
			if (!(*layer)->getLayerState())
				continue;

			//Iterate through all entities
			for (auto& entity : entities) {
				if ((*layer)->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity))
					continue;
				
				//Check for player logic comp
				auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::Movement>(entity);
				if (e_player_comp.has_value()) {
					auto& e_player = e_player_comp.value().get();

					//Skip if script  has not been set
					if (e_player.script.script_path == "")
						continue;

					int move = static_cast<int>(Utility::randFloat() * 3);
					executeScript(e_player.script.script_path, e_player.script.script_id, e_player.script.b_loaded, e_player.script.function)(2, entity, move);
				}

				// Check for shooting comp
				auto e_shoot_comp = NIKE_ECS_MANAGER->getEntityComponent<Shooting::Shooting>(entity);
				if (e_shoot_comp.has_value()) {
					// Get entity's position
					auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
					Vector2f shooter_pos = e_transform_comp.value().get().position;

					// Create bullet
					if (NIKE_INPUT_SERVICE->isKeyTriggered(GLFW_MOUSE_BUTTON_LEFT)) {
					//if (NIKE_INPUT_SERVICE->isKeyTriggered(GLFW_KEY_1)) {
						std::string script_path = "assets/Scripts/createBullet.lua";
						std::string function_name = "createBullet";
						std::string prefab_path = "bullet.prefab";

						// Load Lua Script
						std::string script_id = lua_system->loadScript(script_path);

						// Check if the script is loaded successfully
						if (script_id.empty()) {
							NIKEE_CORE_ERROR("Failed to load script: " + script_path);
						}

						// Execute Lua Script
						sol::protected_function create_bullet_func = lua_system->executeScript(script_id, function_name);


						int layer_id = 0;

						if (!create_bullet_func.valid()) {
							NIKEE_CORE_ERROR("Failed to execute Lua script: " + script_path);
						}
						else {
							// Function was valid 
							sol::protected_function_result result = create_bullet_func(layer_id, prefab_path, shooter_pos.x, shooter_pos.y);


							if (!result.valid()) {
								sol::error err = result;
								NIKEE_CORE_ERROR(fmt::format("Lua error: {}", err.what()));
							}
						}

						
						//create_bullet_func(layer_id, prefab_path);

						//NIKEE_CORE_INFO("Bullet created via Lua script: " + prefab_path);
						//NIKEE_CORE_INFO("Bullet created at x: " + std::to_string(shooter_pos.x) + " y:" + std::to_string(shooter_pos.y));
					}
				}
			}
		}
	}

	//void GameLogic::Manager::init() {

	//	// Variable to store player entity ID
	//	Entity::Type player_entity{};

	//	// Current inefficient method to get player entity
	//	// Locate the player entity by checking each entity's Logic component type
	//	for (auto& entity : entities) {
	//		if (NIKEEngine.checkEntityComponent<Logic::State>(entity)) {
	//			auto& logic_component = NIKEEngine.getEntityComponent<Logic::State>(entity);

	//			// Identify player entity by checking EntityLogicType
	//			if (logic_component.entity_type == EntityLogicType::PLAYER) {
	//				player_entity = entity;
	//				break;  // Stop searching once the player entity is found
	//			}
	//		}
	//	}

	//	// Initialize state machines for entities based on their logic types
	//	for (auto& entity : entities) {
	//		if (NIKEEngine.checkEntityComponent<Logic::State>(entity)) {
	//			auto& logic_component = NIKEEngine.getEntityComponent<Logic::State>(entity);

	//			// Set up a state machine for each entity using its initial state and logic type
	//			state_machines[entity] = std::make_unique<StateMachineManager>(EntityStateType::IDLE, logic_component.entity_type);
	//			auto& state_machine = state_machines[entity];

	//			// Define transition conditions for enemy NPC states
	//			if (logic_component.entity_type == EntityLogicType::ENEMY_NPC) {
	//				// Transition: IDLE to PATROLLING after 5 seconds if player is not within proximity
	//				auto idleToPatrol = [state_machine = state_machine.get()] {
	//					return state_machine->getStateTimer() >= 5.0f;
	//					};

	//				// Transition: IDLE to CHASING if player enters 200 units proximity
	//				auto idleToChase = [entity, player_entity]() {
	//					if (player_entity == Entity::Type{}) return false;  // Ensure player_entity is valid
	//					auto& enemy_transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);
	//					auto& player_transform = NIKEEngine.getEntityComponent<Transform::Transform>(player_entity);
	//					return (enemy_transform.position - player_transform.position).length() <= 200.0f;
	//					};

	//				// Transition: PATROLLING to CHASING if player enters 200 units proximity
	//				auto patrolToChase = [entity, player_entity]() {
	//					if (player_entity == Entity::Type{}) return false;
	//					auto& enemy_transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);
	//					auto& player_transform = NIKEEngine.getEntityComponent<Transform::Transform>(player_entity);
	//					return (enemy_transform.position - player_transform.position).length() <= 200.0f;
	//					};

	//				// Transition: CHASING to IDLE after 5 seconds
	//				auto chaseToIdle = [state_machine = state_machine.get()] {
	//					return state_machine->getStateTimer() >= 5.0f;
	//					};

	//				// Register transitions in the enemy NPC state machine
	//				state_machine->addTransition(EntityStateType::IDLE, EntityStateType::PATROLLING, idleToPatrol);
	//				state_machine->addTransition(EntityStateType::IDLE, EntityStateType::CHASING, idleToChase);
	//				state_machine->addTransition(EntityStateType::PATROLLING, EntityStateType::CHASING, patrolToChase);
	//				state_machine->addTransition(EntityStateType::CHASING, EntityStateType::IDLE, chaseToIdle);
	//			}
	//		}
	//	}
	//}

	//void GameLogic::Manager::update() {
	//	float dt = NIKE_WINDOWS_SERVICE->getDeltaTime();

	//	// Loop through entities
	//	for (auto& entity : entities) {
	//		//Object spawning logic
	//		if (NIKEEngine.checkEntityComponent<GameLogic::ObjectSpawner>(entity)) {
	//			auto& e_mouse = NIKEEngine.getEntityComponent<Input::Mouse>(entity);

	//			if (e_mouse.b_output && (e_mouse.button_type == GLFW_MOUSE_BUTTON_LEFT)) {
	//				auto& camera_system = NIKEEngine.accessSystem<Render::Manager>()->getCamEntity();
	//				float ndcX = (2.0f * e_mouse.button_pos.x) / NIKEEngine.accessWindow()->getWindowSize().x - 1.0f;
	//				float ndcY = 1.0f - (2.0f * e_mouse.button_pos.y) / NIKEEngine.accessWindow()->getWindowSize().y;

	//				Matrix33::Matrix_33 ndc_to_world_xform;
	//				Matrix33::Matrix_33Inverse(ndc_to_world_xform, camera_system->getWorldToNDCXform());

	//				Vector3 ndc_coords = { ndcX, ndcY, 1.0f }; // NDC in homogeneous coordinates
	//				Vector3 world_coords = ndc_coords * ndc_to_world_xform;

	//				// Render start from 3
	//				// Rand between index 7 and 8 (duck objects)
	//				Entity::Type new_entity = NIKEEngine.cloneEntity(7 + std::rand() % 2);
	//				NIKEEngine.getEntityComponent<Transform::Transform>(new_entity).position = { world_coords.x ,world_coords.y };

	//			}
	//		}
	//		else {
	//			// Check if entity has a state machine
	//			if (state_machines.find(entity) == state_machines.end()) {
	//				continue;
	//			}
	//			auto& state_machine = state_machines[entity];
	//			state_machine->update(dt);

	//			// Get the current state and relevant components for enemy NPC logic
	//			auto current_state = state_machine->getCurrentState();
	//			auto& enemy_transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);
	//			auto& dynamics = NIKEEngine.getEntityComponent<Physics::Dynamics>(entity);

	//			switch (current_state) {
	//			case EntityStateType::IDLE:
	//				// In IDLE state, the enemy does not move
	//				dynamics.velocity = { 0.0f, 0.0f };
	//				break;

	//			case EntityStateType::PATROLLING: {
	//				// Define patrol behavior around the idle position
	//				static Vector2f idlePosition = enemy_transform.position;
	//				static Vector2f patrolPoint = idlePosition;

	//				// If enemy reaches patrol point, pick a new random point within 200-unit radius
	//				if ((enemy_transform.position - patrolPoint).length() <= 5.0f) {
	//					std::random_device rd;
	//					std::mt19937 gen(rd());
	//					std::uniform_real_distribution<float> dist(-200.0f, 200.0f);
	//					patrolPoint = idlePosition + Vector2f(dist(gen), dist(gen));
	//				}

	//				Vector2f direction = (patrolPoint - enemy_transform.position).normalize();
	//				dynamics.velocity = direction * dynamics.max_speed;
	//				break;
	//			}

	//			case EntityStateType::CHASING: {
	//				// Chase the player by moving towards the player�s current position
	//				if (player_entity != Entity::Type{}) {  // Ensure player_entity is valid
	//					auto& player_transform = NIKEEngine.getEntityComponent<Transform::Transform>(player_entity);
	//					Vector2f direction = (player_transform.position - enemy_transform.position).normalize();
	//					dynamics.velocity = direction * dynamics.max_speed;
	//				}
	//				break;
	//			}

	//			default:
	//				break;
	//			}
	//		}

	//		/***********************************************
	//		* Check your keypresses here!!!
	//		************************************************/
	//		if (NIKEEngine.checkEntityComponent<Input::Key>(entity)) {
	//			auto& e_key = NIKEEngine.getEntityComponent<Input::Key>(entity);

	//			//Toggle debug
	//			if (NIKEEngine.checkEntityComponent<Render::Debug>(entity)) {
	//				if (e_key.b_output && (e_key.key_type == GLFW_KEY_T)) {
	//					NIKEEngine.accessSystem<Render::Manager>()->debug_mode = !NIKEEngine.accessSystem<Render::Manager>()->debug_mode;
	//				}
	//				continue;
	//			}


	//			// Input for Player
	//			if (NIKEEngine.checkEntityComponent<Move::Movement>(entity) && NIKEEngine.checkEntityComponent<Collision::Collider>(entity)) {
	//				const float movespeed = 300.0f;
	//				Transform::Velocity& velocity = NIKEEngine.getEntityComponent<Transform::Velocity>(entity);
	//				Collision::Collider& collider = NIKEEngine.getEntityComponent<Collision::Collider>(entity);

	//				velocity.velocity.x = 0;
	//				velocity.velocity.y = 0;

	//				// Up
	//				if (e_key.b_output && (e_key.key_type == GLFW_KEY_W) && !collider.top) {
	//					velocity.velocity.y += movespeed;
	//				}
	//				// Left
	//				if (e_key.b_output && (e_key.key_type == GLFW_KEY_A) && !collider.left) {
	//					velocity.velocity.x -= movespeed;
	//				}
	//				// Down
	//				if (e_key.b_output && (e_key.key_type == GLFW_KEY_S) && !collider.bottom) {
	//					velocity.velocity.y -= movespeed;
	//				}
	//				// Right
	//				if (e_key.b_output && (e_key.key_type == GLFW_KEY_D) && !collider.right) {
	//					velocity.velocity.x += movespeed;
	//				}
	//			}
	//		}
	//	}
	//}

	//// Utility functions for game logic, should be moved in future
	//// Generates a random unit vector direction
	//Vector2f GameLogic::Manager::getRandomDirection() {
	//	static std::random_device rd;
	//	static std::mt19937 gen(rd());
	//	static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	//	Vector2f direction(dist(gen), dist(gen));
	//	return direction.normalize(); // Normalize to make it a unit vector
	//}

	//// Searches for and returns the player entity ID if it exists
	//Entity::Type GameLogic::Manager::getPlayerEntity() {
	//	for (auto& entity : NIKE_ECS_MANAGER->getEntities()) {
	//		if (NIKE_ECS_MANAGER->checkEntityComponent<Logic::State>(entity)) {
	//			auto& state = NIKE_ECS_MANAGER->getEntityComponent<Logic::State>(entity);
	//			if (state.entity_type == Logic::EntityLogicType::PLAYER) {
	//				return entity;
	//			}
	//		}
	//	}
	//	// If no player entity is found, return default entity...
	//	return Entity::Type{}; // Need to adjust if no player entity in scene
	//}
}

