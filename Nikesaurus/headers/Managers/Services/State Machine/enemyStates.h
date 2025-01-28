/*****************************************************************//**
 * \file   cEnemyStates.h
 * \brief  Enemy States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once
#ifndef ENEMY_STATES
#define ENEMY_STATES

#include "Managers/Services/State Machine/sStateMachine.h"

namespace NIKE {
	namespace State {
		class IdleState : public StateMachine::Istate
		{
		public:
			IdleState() = default;

			void onEnter(Entity::Type& entity) override;
			void onUpdate(Entity::Type& entity) override;
			void onExit(Entity::Type& entity) override;

		private:

		};

		class AttackState : public StateMachine::Istate
		{
		public:
			AttackState() = default;

			void onEnter(Entity::Type& entity) override;
			void onUpdate(Entity::Type& entity) override;
			void onExit(Entity::Type& entity) override;

		private:

		};

		class ChaseState : public StateMachine::Istate
		{
		public:
			ChaseState() = default;

			void onEnter(Entity::Type& entity) override;
			void onUpdate(Entity::Type& entity) override;
			void onExit(Entity::Type& entity) override;

		private:

		};
	}
}

#endif
