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
	void NIKE::State::ChaseState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter chase state" << endl;
	}

	void NIKE::State::ChaseState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update chase state" << endl;
	}

	void NIKE::State::ChaseState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit chase state" << endl;
	}
}
