/*****************************************************************//**
 * \file   enemyTransitions.cpp
 * \brief  Enemy transitions
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"

// States and transitions
#include "Managers/Transitions/enemyTransitions.h"
#include "Managers/States/enemyStates.h"


namespace NIKE {

	bool Transition::IdleToAttack::isValid()
	{
		auto idle_state = NIKE_FSM_SERVICE->getStateByID<State::IdleState>("Idle");
		if (idle_state) {
			// Checks if within range to transition
			//return idle_state->withinRange(enemy, player); 
		}

		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::IdleToAttack::getNextState()
	{
		return NIKE_FSM_SERVICE->getStateByID<State::IdleState>("Attack");
	}
}

