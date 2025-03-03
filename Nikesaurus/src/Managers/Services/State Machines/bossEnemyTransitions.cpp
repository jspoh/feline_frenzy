/*****************************************************************//**
 * \file   bossEnemyTransitions.cpp
 * \brief  Boss transitions
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"

 // States and transitions
#include "Managers/Services/State Machine/bossEnemyTransitions.h"
#include "Managers/Services/State Machine/bossEnemyStates.h"
#include "Managers/Services/State Machine/enemyUtils.h"

namespace NIKE {

	/*******************************
	* Boss Idle To Boss Attack transition functions
	*****************************/

	bool Transition::BossIdleToBossAttack::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossIdleToBossAttack::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossAttackState>("BossAttack");
	}

	/*******************************
	* Boss Idle To Boss Death transition functions
	*****************************/

	bool Transition::BossIdleToBossDeath::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossIdleToBossDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossDeathState>("BossDeath");
	}

	/*******************************
	* Boss Attack To Boss Idle transition functions
	*****************************/

	bool Transition::BossAttackToBossIdle::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossAttackToBossIdle::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossIdleState>("BossIdle");
	}

	/*******************************
	* Boss Attack To Boss Death transition functions
	*****************************/

	bool Transition::BossAttackToBossDeath::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossAttackToBossDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossDeathState>("BossDeath");
	}
}


