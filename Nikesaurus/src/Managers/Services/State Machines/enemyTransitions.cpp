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
#include "Managers/Services/State Machine/enemyTransitions.h"
#include "Managers/Services/State Machine/enemyStates.h"
#include "Systems/GameLogic/sysEnemy.h"


namespace NIKE {

	bool Transition::IdleToAttack::isValid(Entity::Type& entity) const
	{
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& other_entity : NIKE_ECS_MANAGER->getAllEntities()) {
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
			// Somehow e_player_comp is getting nullptr
			if (e_player_comp.has_value())
			{
				// If entity has the gamelogic::ilogic component, and within range of enemy
				if (Enemy::withinRange(entity, other_entity)){
					return true;
				}
			}

		}

		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::IdleToAttack::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::AttackState>("Attack");
	}
}

