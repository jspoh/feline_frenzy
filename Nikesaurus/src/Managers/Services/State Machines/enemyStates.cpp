/*****************************************************************//**
 * \file   cEnemyStates.cpp
 * \brief  Enemy States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/State Machine/enemyStates.h"
#include "Managers/Services/State Machine/enemyTransitions.h"
#include "Systems/GameLogic/sysEnemy.h"
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
		if (e_enemy_comp.has_value()) {

			auto& enemy_comp = e_enemy_comp.value().get();

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

	/*******************************
	* Chase State functions
	*****************************/

	State::ChaseState::ChaseState() : cell_offset{ 15.0f }, enemy_speed{ 1000.0f }
	{
		// Add transitions here
		addTransition("ChaseToAttack", std::make_shared<Transition::ChaseToAttack>());
		addTransition("ChaseToIdle", std::make_shared<Transition::ChaseToIdle>());
	}

	void NIKE::State::ChaseState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter chase state" << endl;
	}

	void NIKE::State::ChaseState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{

		// cout << "update chase state" << endl;
		auto path = NIKE_MAP_SERVICE->getPath(entity);
		auto e_transform_enemy = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);

		if (e_transform_enemy.has_value() && !path.path.empty())
		{
			//Get next cell
			auto const& next_cell = path.path.front();
			auto& e_transform = e_transform_enemy.value().get();

			//Check if entity has arrived near destination
			if ((next_cell.position - e_transform.position).length() > cell_offset) {

				//Direction of next cell
				float dir = atan2((next_cell.position.y - e_transform.position.y), (next_cell.position.x - e_transform.position.x));

				//Apply force to entity
				auto dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
				if (dynamics.has_value()) {
					dynamics.value().get().force = { cos(dir) * enemy_speed, sin(dir) * enemy_speed };
					// cout << "force added" << endl;
				}

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
					// No flip for up-right
					flipX(entity, false);   
					setLastDirection(entity, 1);
				}
				else if (dir >= 3 * M_PI / 8 && dir < 5 * M_PI / 8) {
					// Moving up
					animationStart(entity, 0, 3);
					animationEnd(entity, 9, 3);
					// No flip for up
					flipX(entity, false);
					setLastDirection(entity, 2);
				}
				else if (dir >= 5 * M_PI / 8 && dir < 7 * M_PI / 8) {
					// Moving up-left (diagonal)
					animationStart(entity, 0, 2);
					animationEnd(entity, 9, 2);
					// Flip for up-left
					flipX(entity, true);
					setLastDirection(entity, 3);
				}
				else if (dir >= -3 * M_PI / 8 && dir < -M_PI / 8) {
					// Moving down-right (diagonal)
					animationStart(entity, 0, 1);
					animationEnd(entity, 9, 1);
					// No flip for down-right
					flipX(entity, false);
					setLastDirection(entity, 4);
				}
				else if (dir >= -5 * M_PI / 8 && dir < -3 * M_PI / 8) {
					// Moving down
					animationStart(entity, 0, 0);
					animationEnd(entity, 9, 0);
					// No flip for down
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
			else {
				// cout << "force added, path popped front" << endl;
				path.path.pop_front();
			}
		}

		else {

			//Marked path as finished
			// cout << "force added, path finished" << endl;
			path.b_finished = true;
		}

	}

	void NIKE::State::ChaseState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit chase state" << endl;
	}
}
