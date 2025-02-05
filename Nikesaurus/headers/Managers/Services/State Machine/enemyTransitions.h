/*****************************************************************//**
 * \file   enemyTransitions.h
 * \brief  Enemy Transitions
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once
#ifndef ENEMY_TRANSITION
#define ENEMY_TRANSITION

#include "Managers/Services/State Machine/sStateMachine.h"

namespace NIKE {
	namespace Transition {

		/****************************
		* Idle Transitions
		*****************************/
		class IdleToEnemyAttack : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};

		class IdleToEnemyChase : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};

		class IdleToEnemyDeath : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};

		/****************************
		* Enemy Attack Transitions
		*****************************/
		class EnemyAttackToIdle : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};

		class EnemyAttackToEnemyChase : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};

		class EnemyAttackToEnemyDeath : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};

		/****************************
		* EnemyChaseState Transitions
		*****************************/
		class EnemyChaseToEnemyAttack : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};

		class EnemyChaseToIdle : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};

		class EnemyChaseToEnemyDeath : public StateMachine::Itransition {
			// Determines if the transition to the next state is valid.
			bool isValid(Entity::Type& entity) const override;

			// Returns the state that should be transitioned to, if valid.
			std::shared_ptr<StateMachine::Istate> getNextState() const override;
		};

		/************************************************************************************
		* Enemy Death State Transitions (No transitions for now unless theres gonna be a revivie or sum)
		***********************************************************************/
	}
}

#endif