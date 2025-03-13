/*****************************************************************//**
 * \file   bossEnemyStates.h
 * \brief  Boss States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once
#ifndef BOSS_STATES
#define BOSS_STATES

#include "Managers/Services/State Machine/sStateMachine.h"
#include "Managers/ECS/mEntity.h"
#include "Managers/ECS/mSystem.h"
#include "Components/cPhysics.h"

namespace NIKE {
	namespace State {

		// Utility functions
		std::string getSpriteSheet(const std::string& fsm_state, const std::string& element);

		class BossIdleState : public StateMachine::Istate
		{
		public:
			BossIdleState();

			void onEnter(Entity::Type& entity) override;
			void onUpdate(Entity::Type& entity) override;
			void onExit(Entity::Type& entity) override;

			/***********************
			* SFX handling
			************************/
			void playSFX(Entity::Type& entity, bool play_or_no) override;

		private:

		};

		class BossAttackState : public StateMachine::Istate, public Events::IEventListener<Physics::CollisionEvent>
		{
		public:
			BossAttackState();

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
			void updateBossAttackAnimation([[maybe_unused]] Entity::Type& entity);
		};

		class BossChaseState : public StateMachine::Istate
		{
		public:
			BossChaseState();

			void onEnter(Entity::Type& entity) override;
			void onUpdate(Entity::Type& entity) override;
			void onExit(Entity::Type& entity) override;

			/***********************
			* SFX handling
			************************/
			void playSFX(Entity::Type& entity, bool play_or_no) override;

		private:
			float cell_offset;
			float boss_speed;
			void updateBossChaseAnimation([[maybe_unused]] Entity::Type& entity, float dir);

		};

		class BossDeathState : public StateMachine::Istate
		{
		public:
			BossDeathState();

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