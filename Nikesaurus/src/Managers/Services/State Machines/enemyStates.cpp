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
#include "Systems/GameLogic/sysEnemy.h"
#include "Core/Engine.h"

namespace NIKE {

	/*******************************
	* Idle State functions
	*****************************/

	void State::IdleState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;

	}

	void State::IdleState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update Idle State" << endl;
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& other_entity : NIKE_ECS_MANAGER->getAllEntities()) {
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
			// If entity has the gamelogic::ilogic component, and within range of enemy
			if (e_player_comp.has_value() && Enemy::withinRange(entity, other_entity))
			{
				auto attack_state = NIKE_FSM_SERVICE->getStateByID("Attack");
				NIKE_FSM_SERVICE->changeState(attack_state, entity);
			}

		}
	}

	void State::IdleState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
	}

	/*******************************
	* Attack State functions
	*****************************/

	void NIKE::State::AttackState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter attack state" << endl;
	}

	void NIKE::State::AttackState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// Within range returns true, shoot bullet
		// shootBullet(entity, player);
		// cout << "update attack state" << endl;
	}

	void NIKE::State::AttackState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit attack state" << endl;
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
