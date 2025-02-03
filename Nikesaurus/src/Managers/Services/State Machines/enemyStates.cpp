/*****************************************************************//**
 * \file   enemyStates.cpp
 * \brief  Enemy States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/State Machine/enemyStates.h"
#include "Managers/Services/State Machine/enemyTransitions.h"
#include "Managers/Services/State Machine/enemyUtils.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Core/Engine.h"

namespace NIKE {

	/*******************************
	* Idle State functions
	*****************************/

	State::IdleState::IdleState()
	{
		// Add transitions here
		addTransition("IdleToAttack", std::make_shared<Transition::IdleToAttack>());
		addTransition("IdleToChase", std::make_shared<Transition::IdleToChase>());
		addTransition("IdleToDeath", std::make_shared<Transition::IdleToDeath>());
	}

	void State::IdleState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;
		//if (!checkTransitionExist("IdleToAttack"))
		//{
		//	addTransition("IdleToAttack", std::make_shared<Transition::IdleToAttack>());
		//}
	}

	void State::IdleState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update Idle State" << endl;
	}

	void State::IdleState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
		// removeTransition("IdleToAttack");
	}

	/*******************************
	* Attack State functions
	*****************************/

	State::AttackState::AttackState()
	{
		// Add transitions here
		addTransition("AttackToIdle", std::make_shared<Transition::AttackToIdle>());
		addTransition("AttackToChase", std::make_shared<Transition::AttackToChase>());
		addTransition("AttackToDeath", std::make_shared<Transition::AttackToDeath>());

		// Register the Manager as a listener for collision events
		std::shared_ptr<AttackState> attack_state_wrapped(this, [](AttackState*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Physics::CollisionEvent>(attack_state_wrapped);
	}

	void NIKE::State::AttackState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter attack state" << endl;
		//if (!checkTransitionExist("AttackToIdle"))
		//{
		//	addTransition("AttackToIdle", std::make_shared<Transition::IdleToAttack>());
		//}
	}

	void NIKE::State::AttackState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// Check for attack comp
		auto e_enemy_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(entity);
		auto e_enemy_dyna = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
		if (e_enemy_comp.has_value() && e_enemy_dyna.has_value()) {

			auto& enemy_comp = e_enemy_comp.value().get();
			auto& dyna_comp = e_enemy_dyna.value().get();

			// If shot on cooldown
			if (enemy_comp.last_shot_time <= enemy_comp.cooldown) {
				// Accumulate time since last shot
				enemy_comp.last_shot_time += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
			}

			// Look for entity w player component
			for (auto& other_entity : NIKE_ECS_MANAGER->getAllEntities()) {
				// Look for entity w player component, do like this first, when meta data is out, no need iterate through
				auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
				// If player entity exists
				if (e_player_comp.has_value()) {
					// Check if player is within range & shot not on cooldown
					if (enemy_comp.last_shot_time >= enemy_comp.cooldown) {
						// Stop enemy when they shooting
						dyna_comp.force = { 0,0 };
						dyna_comp.velocity = { 0,0 };
						// Shoot bullet towards player pos from enemy pos
						Enemy::shootBullet(entity, other_entity);

						// Reset the last shot time after shooting
						enemy_comp.last_shot_time = 0.f;
					}
				}
			}
		}
		// cout << "update attack state" << endl;
	}

	void NIKE::State::AttackState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "exit attack state" << endl;
		// removeTransition("AttackToIdle");
	}

	void State::AttackState::onEvent(std::shared_ptr<Physics::CollisionEvent> event)
	{
		// Ensure entities exist and handle the collision
		if (NIKE_ECS_MANAGER->checkEntity(event->entity_a) && NIKE_ECS_MANAGER->checkEntity(event->entity_b)) {
			Interaction::handleCollision(event->entity_a, event->entity_b);
		}
	}

	/*******************************
	* Chase State functions
	*****************************/

	State::ChaseState::ChaseState() : cell_offset{ 15.0f }, enemy_speed{ 1000.0f }
	{
		// Add transitions here
		addTransition("ChaseToAttack", std::make_shared<Transition::ChaseToAttack>());
		addTransition("ChaseToIdle", std::make_shared<Transition::ChaseToIdle>());
		addTransition("ChaseToDeath", std::make_shared<Transition::ChaseToDeath>());
	}

	void NIKE::State::ChaseState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter chase state" << endl;
	}

	void State::ChaseState::onUpdate(Entity::Type& entity)
	{
		for (const auto& other_entity : NIKE_ECS_MANAGER->getAllEntities())
		{
			// Getting components from player and enemy entities
			auto e_player_game_logic = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
			auto e_player_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(other_entity);

			auto e_enemy_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
			auto e_enemy_dyna = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
			if (e_enemy_transform.has_value() && e_player_game_logic.has_value() && e_player_transform.has_value() && e_enemy_dyna.has_value())
			{
				auto& player_transform = e_player_transform.value().get();

				// Get enemy position as starting cell
				auto end = NIKE_MAP_SERVICE->getCellIndexFromCords(player_transform.position);
				if (!end.has_value()) return;

				// Move the entity along the computed path
				Enemy::moveAlongPath(entity, end.value().x, end.value().y, enemy_speed, cell_offset);
			}


			// Update animation based on movement direction
			if (e_enemy_dyna.has_value() && e_enemy_dyna.value().get().force.length() > 0.01f) {
				float dir = atan2(e_enemy_dyna.value().get().force.y, e_enemy_dyna.value().get().force.x);
				updateChaseAnimation(entity, dir);
			}

		}
	}


	void NIKE::State::ChaseState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit chase state" << endl;
	}

	void State::ChaseState::updateChaseAnimation(Entity::Type& entity, float& dir)
	{
		if (dir >= -M_PI / 8 && dir < M_PI / 8) {
			// Moving right
			animationStart(entity, 0, 1);
			animationEnd(entity, 9, 1);
			flipX(entity, false);
			setLastDirection(entity, 0);
		}
		else if (dir >= M_PI / 8 && dir < 3 * M_PI / 8) {
			// Moving up-right (diagonal)
			animationStart(entity, 0, 2);
			animationEnd(entity, 9, 2);
			flipX(entity, false);
			setLastDirection(entity, 1);
		}
		else if (dir >= 3 * M_PI / 8 && dir < 5 * M_PI / 8) {
			// Moving up
			animationStart(entity, 0, 0);
			animationEnd(entity, 9, 0);
			flipX(entity, false);
			setLastDirection(entity, 2);
		}
		else if (dir >= 5 * M_PI / 8 && dir < 7 * M_PI / 8) {
			// Moving up-left (diagonal)
			animationStart(entity, 0, 2);
			animationEnd(entity, 9, 2);
			flipX(entity, true);
			setLastDirection(entity, 3);
		}
		else if (dir >= -3 * M_PI / 8 && dir < -M_PI / 8) {
			// Moving down-right (diagonal)
			animationStart(entity, 0, 1);
			animationEnd(entity, 9, 1);
			flipX(entity, false);
			setLastDirection(entity, 4);
		}
		else if (dir >= -5 * M_PI / 8 && dir < -3 * M_PI / 8) {
			// Moving down
			animationStart(entity, 0, 0);
			animationEnd(entity, 9, 0);
			flipX(entity, false);
			setLastDirection(entity, 5);
		}
		else if (dir >= -7 * M_PI / 8 && dir < -5 * M_PI / 8) {
			// Moving down-left (diagonal)
			animationStart(entity, 0, 1);
			animationEnd(entity, 9, 1);
			flipX(entity, true);
			setLastDirection(entity, 6);
		}
		else {
			// Moving left
			animationStart(entity, 0, 1);
			animationEnd(entity, 9, 1);
			flipX(entity, true);
			setLastDirection(entity, 7);
		}
	}

	/*******************************
	* Death State functions
	*****************************/

	State::DeathState::DeathState()
	{
		// Add transitions here

		// Register the Manager as a listener for collision events
		std::shared_ptr<DeathState> death_state_wrapped(this, [](DeathState*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Physics::CollisionEvent>(death_state_wrapped);
	}

	void State::DeathState::onEnter([[maybe_unused]] Entity::Type& entity) {
		// Play death animation
		animationStart(entity, 0, 8);
		animationEnd(entity, 2, 8);
		flipX(entity, false);
	}
	void State::DeathState::onUpdate([[maybe_unused]] Entity::Type& entity) {
		auto animation_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
		if (animation_comp.has_value())
		{
			// Use delta time to let animation play before deleting entity
			static float dt = 0.0f;
			dt += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
			if (dt >= 0.5f) { 
				NIKE_ECS_MANAGER->markEntityForDeletion(entity);
				// Reset delta time
				dt = 0.f;
			}
		}
	}
	void State::DeathState::onExit([[maybe_unused]] Entity::Type& entity){
		
	}
	void State::DeathState::onEvent(std::shared_ptr<Physics::CollisionEvent> event)
	{
		// Ensure entities exist and handle the collision
		if (NIKE_ECS_MANAGER->checkEntity(event->entity_a) && NIKE_ECS_MANAGER->checkEntity(event->entity_b)) {
			Interaction::handleCollision(event->entity_a, event->entity_b);
		}
	}
}


