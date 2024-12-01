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

namespace NIKE {

	void GameLogic::Manager::init() {
	}

	//void GameLogic::Manager::registerLuaSystem(std::shared_ptr<Lua::ILuaBind> system) {
	//	//Add system to lua
	//	NIKE_LUA_SERVICE->registerLuaSystem(system);
	//}

	// sol::protected_function GameLogic::Manager::executeScript(std::string const& file_path, std::string& script_id, bool& b_loaded, std::string const& function) {
		//Run script
		//if (script_id == "") {
		//	script_id = NIKE_LUA_SERVICE->loadScript(file_path);
		//	b_loaded = true;
		//	return NIKE_LUA_SERVICE->executeScript(script_id, function);
		//}
		//else if (b_loaded) {
		//	return NIKE_LUA_SERVICE->executeScript(script_id, function);
		//}
		//else {
		//	NIKE_LUA_SERVICE->reloadScript(script_id);
		//	b_loaded = true;
		//}
	// 	return 0;
	// }

	void GameLogic::Manager::update() {
		//NIKEE_CORE_WARN("Mouse X: {} Y: {}", NIKE_INPUT_SERVICE.get()->getMouseWorldPos().x, NIKE_INPUT_SERVICE.get()->getMouseWorldPos().y);
		//Get layers
		auto& layers = NIKE_SCENES_SERVICE->getLayers();

		//Reverse Iterate through layers
		for (auto layer = layers.rbegin(); layer != layers.rend(); layer++) {

			//Skip inactive layer
			if (!(*layer)->getLayerState())
				continue;

			//Iterate through all entities
			for (auto& entity : entities) {
				if ((*layer)->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity))
					continue;

				//Check for player logic comp
				auto e_logic_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(entity);
				if (e_logic_comp.has_value()) {
					auto& e_logic = e_logic_comp.value().get();

					//Skip if script id has not been set

					// if (e_player.script.script_id == "")
					// 	continue;

					// 	int move = static_cast<int>(Utility::randFloat() * 3);

					// 	std::filesystem::path path = NIKE_ASSETS_SERVICE->getAssetPath("assets/Scripts/player.lua");

					// 	sol::load_result result = NIKE_LUA_SERVICE->loadScript(path);
					// 	/*NIKE_LUA_SERVICE->executeScript(e_player.script.script_path, e_player.script.script_id, e_player.script.b_loaded, e_player.script.function)(2, entity, move);*/
					// 	NIKE_LUA_SERVICE->executeScript(result, e_player.script.function,3, 2, entity, move);
					// }
						////Execute script
						//NIKE_LUA_SERVICE->executeScript("test.lua", "update", 0);
						//Skip if script id has not been set
					if (e_logic.script.script_id == "")
						continue;

					//Default named argument passed to the script
					e_logic.script.named_args["entity"] = entity;

					//Execute script
					NIKE_LUA_SERVICE->executeScript(e_logic.script);
				}

				// Check for shooting comp
				auto e_shoot_comp = NIKE_ECS_MANAGER->getEntityComponent<Shooting::Shooting>(entity);
				if (e_shoot_comp.has_value()) {

					// Get shooting comp
					auto& shoot_comp = e_shoot_comp.value().get();

					// If shooting is on cooldown
					if (shoot_comp.last_shot_time < shoot_comp.cooldown) {
						// Accumulate time since last shot
						shoot_comp.last_shot_time += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
					}

					// Create bullet
					if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_MOUSE_BUTTON_1)) {
						// Cooldown
						if (shoot_comp.last_shot_time >= shoot_comp.cooldown) {
							// Get entity's position
							auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
							Vector2f shooter_pos = e_transform_comp.value().get().position;

							// Shoot bullet towards cursor position from player pos
							shootCursor(entity);

							// Reset the last shot time after shooting
							shoot_comp.last_shot_time = 0.f;
						}
						else {
							// Cooldown not up
							NIKEE_CORE_INFO("Cannot shoot yet. Time until next shot: " + std::to_string(shoot_comp.cooldown - shoot_comp.last_shot_time));
						}
					}
				}

				//Check for despawn comp
				//auto e_despawn_comp = NIKE_ECS_MANAGER->getEntityComponent<Despawn::Lifetime>(entity);
				//if (e_despawn_comp.has_value()) {
				//	auto& e_despawn = e_despawn_comp.value().get();

				//	// update current lifetime
				//	e_despawn.current_lifetime += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();

				//	NIKEE_CORE_WARN("Current Lifetime: {}", e_despawn.current_lifetime);
				//	NIKEE_CORE_WARN("Max Lifetime: {}", e_despawn.max_lifetime);

				//	// if current lifetime > max lifetime, mark for deletion
				//	if (e_despawn.current_lifetime >= e_despawn.max_lifetime) {
				//		if (NIKE_ECS_MANAGER->checkEntity(entity)) {
				//			NIKE_ECS_MANAGER->markEntityForDeletion(entity);
				//		}
				//	}
				//}

				// Destroy all entities that are marked for deletion
				NIKE_ECS_MANAGER->destroyMarkedEntities();
			}
		}
	}
	void GameLogic::Manager::shootCursor(const Entity::Type& player_entity) {
		// Get player components (Making copies to avoid dangling pointer warning)
		const auto p_shoot_comp = NIKE_ECS_MANAGER->getEntityComponent<Shooting::Shooting>(player_entity);
		const auto& player_shoot_comp = p_shoot_comp.value().get();
		const auto p_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player_entity);
		const auto& player_transform_comp = p_transform_comp.value().get();

		// Create entity for bullet
		Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity(player_shoot_comp.layer);

		// Load entity from prefab
		NIKE_SERIALIZE_SERVICE->loadEntityFromFile(bullet_entity, NIKE_ASSETS_SERVICE->getAssetPath("bullet.prefab").string());

		// Calculate direction for bullet (Mouse Pos - Player Pos)
		const Vector2f& player_pos = player_transform_comp.position;
		const Vector2f mouse_pos = { NIKE_INPUT_SERVICE.get()->getMouseWorldPos().x, -NIKE_INPUT_SERVICE.get()->getMouseWorldPos().y };
		Vector2f direction = mouse_pos - player_pos;
		direction.normalize();

		// Offset spawn position of bullet
		const float& offset = player_shoot_comp.offset;
		const Vector2f bullet_pos = player_pos + (direction * offset);

		// Set bullet's position
		auto bullet_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(bullet_entity);
		if (bullet_transform_comp.has_value()) {
			bullet_transform_comp.value().get().position = bullet_pos;
		}

		// Set bullet physics
		auto bullet_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(bullet_entity);
		if (bullet_physics_comp.has_value()) {
			// Set force
			bullet_physics_comp.value().get().force = { direction.x, direction.y };
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


