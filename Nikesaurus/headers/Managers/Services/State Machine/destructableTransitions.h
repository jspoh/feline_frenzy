/*****************************************************************//**
 * \file   destructableTransitions.h
 * \brief  Destructable Transitions
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once
#ifndef DESTRUCTABLE_TRANSITION
#define DESTRUCTABLE_TRANSITION

#include "Managers/Services/State Machine/sStateMachine.h"

namespace NIKE {
	namespace Transition {
		/****************************
		* Default To Destructable death Transition
		 *****************************/
		class DefaultToDestructableDeath : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};
	}

}

 

#endif