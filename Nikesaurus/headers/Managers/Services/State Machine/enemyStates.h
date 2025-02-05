/*****************************************************************//**
 * \file   enemyStates.h
 * \brief  Enemy States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once
#ifndef ENEMY_STATES
#define ENEMY_STATES

#include "Managers/Services/State Machine/sStateMachine.h"
#include "Managers/ECS/mEntity.h"
#include "Managers/ECS/mSystem.h"
#include "Components/cPhysics.h"

namespace NIKE {
	namespace State {
		class IdleState : public StateMachine::Istate
		{
		public:
			IdleState();

			void onEnter(Entity::Type& entity) override;
			void onUpdate(Entity::Type& entity) override;
			void onExit(Entity::Type& entity) override;

			/***********************
			* SFX handling
			************************/
			void playSFX(Entity::Type& entity, bool play_or_no) override;

		private:

		};

		class AttackState : public StateMachine::Istate, public Events::IEventListener<Physics::CollisionEvent>
		{
		public:
			AttackState();

			void onEnter(Entity::Type& entity) override;
			void onUpdate(Entity::Type& entity) override;
			void onExit(Entity::Type& entity) override;

			/***********************
			* SFX handling
			************************/
			void playSFX(Entity::Type& entity, bool play_or_no) override;


			// Event handling for collisions
			void onEvent(std::shared_ptr<Physics::CollisionEvent> event) override;

		private:
			void updateAttackAnimation(Entity::Type& entity);
		};

		class ChaseState : public StateMachine::Istate
		{
		public:
			ChaseState();

			void onEnter(Entity::Type& entity) override;
			void onUpdate(Entity::Type& entity) override;
			void onExit(Entity::Type& entity) override;

			/***********************
			* SFX handling
			************************/
			void playSFX(Entity::Type& entity, bool play_or_no) override;

		private:
			//Acceptable offset per cell
			float cell_offset;
			float enemy_speed;

			void updateChaseAnimation(Entity::Type& entity, float& dir);

		};

		class DeathState : public StateMachine::Istate
		{
		public:
			DeathState();

			void onEnter(Entity::Type& entity) override;
			void onUpdate(Entity::Type& entity) override;
			void onExit(Entity::Type& entity) override;

			/***********************
			* SFX handling
			************************/
			void playSFX(Entity::Type& entity, bool play_or_no) override;

		private:

		};
	}
}

#endif
