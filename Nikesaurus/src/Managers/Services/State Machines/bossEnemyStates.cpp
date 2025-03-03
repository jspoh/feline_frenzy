/*****************************************************************//**
 * \file   bossEnemyStates.cpp
 * \brief  Boss States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/State Machine/bossEnemyStates.h"
#include "Managers/Services/State Machine/enemyUtils.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Core/Engine.h"

 // Transitions
#include "Managers/Services/State Machine/bossEnemyTransitions.h"
#include "Managers/Services/State Machine/destructableTransitions.h"

namespace NIKE {
	/*******************************
	* Boss Idle State functions
	*****************************/

	State::BossIdleState::BossIdleState()
	{
		// Add transitions here
		addTransition("BossIdleToBossAttack", std::make_shared<Transition::BossIdleToBossAttack>());
		addTransition("BossIdleToBossDeath", std::make_shared<Transition::BossIdleToBossDeath>());
	}

	void State::BossIdleState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;
		//if (!checkTransitionExist("IdleToAttack"))
		//{
		//	addTransition("IdleToAttack", std::make_shared<Transition::IdleToAttack>());
		//}
	}

	void State::BossIdleState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update Idle State" << endl;
	}

	void State::BossIdleState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
		// removeTransition("IdleToAttack");
	}

	void State::BossIdleState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	{
	}

	/*******************************
	* Boss Attack State functions
	*****************************/

	State::BossAttackState::BossAttackState()
	{
		// Add transitions here
		addTransition("BossAttackToBossIdle", std::make_shared<Transition::BossAttackToBossIdle>());
		addTransition("BossAttackToBossDeath", std::make_shared<Transition::BossAttackToBossDeath>());
	}

	void State::BossAttackState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;
		//if (!checkTransitionExist("IdleToAttack"))
		//{
		//	addTransition("IdleToAttack", std::make_shared<Transition::IdleToAttack>());
		//}
	}

	void State::BossAttackState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update Idle State" << endl;
	}

	void State::BossAttackState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
		// removeTransition("IdleToAttack");
	}

	void State::BossAttackState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	{
	}

	/*******************************
	* Boss Death State functions
	*****************************/

	State::BossDeathState::BossDeathState()
	{
		// Add transitions here
	}

	void State::BossDeathState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;
		//if (!checkTransitionExist("IdleToAttack"))
		//{
		//	addTransition("IdleToAttack", std::make_shared<Transition::IdleToAttack>());
		//}
	}

	void State::BossDeathState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update Idle State" << endl;
	}

	void State::BossDeathState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
		// removeTransition("IdleToDeath");
	}

	void State::BossDeathState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	{
	}
}