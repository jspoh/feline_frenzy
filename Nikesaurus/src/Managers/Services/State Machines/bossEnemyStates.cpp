/*****************************************************************//**
 * \file   bossEnemyStates.cpp
 * \brief  Boss States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/State Machine/bossEnemyStates.h"
#include "Managers/Services/State Machine/enemyUtils.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Core/Engine.h"

 // Transitions
#include "Managers/Services/State Machine/bossEnemyTransitions.h"
#include "Managers/Services/State Machine/destructableTransitions.h"

namespace NIKE {
	/*******************************
	* Boss Idle State functions
	*****************************/

	State::BossIdleState::BossIdleState()
	{
		// Add transitions here
		addTransition("BossIdleToBossAttack", std::make_shared<Transition::BossIdleToBossAttack>());
		addTransition("BossIdleToBossDeath", std::make_shared<Transition::BossIdleToBossDeath>());
	}

	void State::BossIdleState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;
		//if (!checkTransitionExist("IdleToAttack"))
		//{
		//	addTransition("IdleToAttack", std::make_shared<Transition::IdleToAttack>());
		//}
	}

	void State::BossIdleState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update Idle State" << endl;
	}

	void State::BossIdleState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
		// removeTransition("IdleToAttack");
	}

	void State::BossIdleState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	{
	}

	/*******************************
	* Boss Attack State functions
	*****************************/

	State::BossAttackState::BossAttackState()
	{
		// Add transitions here
		addTransition("BossAttackToBossIdle", std::make_shared<Transition::BossAttackToBossIdle>());
		addTransition("BossAttackToBossDeath", std::make_shared<Transition::BossAttackToBossDeath>());
	}

	void State::BossAttackState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;
		//if (!checkTransitionExist("IdleToAttack"))
		//{
		//	addTransition("IdleToAttack", std::make_shared<Transition::IdleToAttack>());
		//}
	}

	void State::BossAttackState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update Idle State" << endl;
		// Check for attack comp
		auto e_enemy_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(entity);
		auto e_enemy_dyna = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
		auto e_enemy_health = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
		auto e_enemy_ani = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
		if (e_enemy_comp.has_value() && e_enemy_dyna.has_value() && e_enemy_health.has_value() && e_enemy_ani.has_value()) {

			auto& enemy_comp = e_enemy_comp.value().get();
			auto& dyna_comp = e_enemy_dyna.value().get();

			// If shot on cooldown
			if (enemy_comp.last_shot_time <= enemy_comp.cooldown) {
				// Accumulate time since last shot
				enemy_comp.last_shot_time += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
			}

			// Look for entity w player component
			for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
				// Look for entity w player component, do like this first, when meta data is out, no need iterate through
				auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(player);
				// If player entity exists
				if (e_player_comp.has_value()) {
					// Check if player is within range & shot not on cooldown
					if (enemy_comp.last_shot_time >= enemy_comp.cooldown) {
						// Stop enemy when they shooting
						dyna_comp.force = { 0,0 };
						dyna_comp.velocity = { 0,0 };
						// Shoot bullet towards player pos from enemy pos
						Enemy::bossShoot(entity, player);
						// TODO: ADD ANIMATIONS
						// updateEnemyAttackAnimation(entity);
						auto e_audio_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
						if (e_audio_comp.has_value())
						{
							// TODO: ADD SFX
							// playSFX(entity, false);
						}
						// Reset the last shot time after shooting
						enemy_comp.last_shot_time = 0.f;
					}
				}
			}
		}
	}

	void State::BossAttackState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
		// removeTransition("IdleToAttack");
	}

	void State::BossAttackState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	{
	}

	/*******************************
	* Boss Death State functions
	*****************************/

	State::BossDeathState::BossDeathState()
	{
		// Add transitions here
	}

	void State::BossDeathState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;
		// Stop enemy from moving
		auto dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
		if (dynamics.has_value()) {

			dynamics.value().get().max_speed = { 0.0f };
		}

		// Play EnemyDeathState animation
		Interaction::animationSet(entity, 0, 8, 10, 8);
		Interaction::flipX(entity, false);
		playSFX(entity, true);
	}

	void State::BossDeathState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update Idle State" << endl;
		auto animation_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
		if (animation_comp.has_value())
		{
			if (animation_comp.value().get().completed_animations >= 1) {
				playSFX(entity, false);
				NIKE_METADATA_SERVICE->destroyEntity(entity);
			}
		}
	}

	void State::BossDeathState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
		// removeTransition("IdleToDeath");
	}

	void State::BossDeathState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	{
		if (play_or_no) {
			// Temporary hardcoded SFX
			Interaction::playOneShotSFX(entity, "EnemyDeath1.wav", "EnemySFX", 1.0f, 1.0f);

			// Delay for 0.5 seconds using engine's delta time (careful busy-wait loop)
			float secondsToDelay = 0.5f;
			float currentDelay = 0.0f;
			while (currentDelay < secondsToDelay) {
				currentDelay += NIKE_WINDOWS_SERVICE->getDeltaTime();
			}
		}
	}
}