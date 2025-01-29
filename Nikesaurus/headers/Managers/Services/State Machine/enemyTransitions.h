/*****************************************************************//**
 * \file   enemyTransitions.h
 * \brief  Enemy Transitions
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once
#ifndef ENEMY_TRANSITION
#define ENEMY_TRANSITION

#include "Managers/Services/State Machine/sStateMachine.h"

namespace NIKE {
	namespace Transition {
		class IdleToAttack : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};

		class AttackToIdle : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};
	}
}

#endif