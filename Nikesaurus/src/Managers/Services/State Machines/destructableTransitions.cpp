/*****************************************************************//**
 * \file   destructableTransitions.cpp
 * \brief  Destructable Transitions
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/State Machine/enemyUtils.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Core/Engine.h"
#include "Managers/Services/State Machine/destructableTransitions.h"
#include "Managers/Services/State Machine/destructableStates.h"


namespace NIKE {

    bool Transition::DefaultToDestructableDeath::isValid(Entity::Type& entity) const {
        const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
        
        // Checks
        if (!target_health_comp.has_value()) {
            return false;
        }

        const auto& health = target_health_comp.value().get();
        if (health.lives > 0) {
            return false;
        }

        const auto& tags = NIKE_METADATA_SERVICE->getEntityTags(entity);
        if (tags.find("objects") != tags.end() && health.lives <= 0) {
            return true;
        }

        return false;
    }



	std::shared_ptr<StateMachine::Istate> NIKE::Transition::DefaultToDestructableDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::DestructableDeathState>("DestructableDeath");
	}

}


