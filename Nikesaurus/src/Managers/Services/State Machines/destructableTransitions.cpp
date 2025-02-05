/*****************************************************************//**
 * \file   destructableTransitions.cpp
 * \brief  Destructable Transitions
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/State Machine/enemyUtils.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Core/Engine.h"
#include "Managers/Services/State Machine/destructableTransitions.h"
#include "Managers/Services/State Machine/destructableStates.h"


namespace NIKE {

	bool Transition::IdleToDestructableDeath::isValid(Entity::Type& entity) const {
		const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
		if (target_health_comp.has_value())
		{
			if (target_health_comp.value().get().lives <= 0) {
				return true;
			}
		}
		return false;
	}


	std::shared_ptr<StateMachine::Istate> NIKE::Transition::IdleToDestructableDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::DestructableDeathState>("DestructableDeath");
	}

}


