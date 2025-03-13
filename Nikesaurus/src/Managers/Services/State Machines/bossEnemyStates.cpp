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
#include "Managers/Services/State Machine/enemyStates.h"

namespace NIKE {

	/*******************************
	* Utility functions
	*****************************/

	// Lookup table to take the correct spritesheet
	std::unordered_map<std::string, std::string> state_to_spritesheet = {
		{"BossIdle", "Boss_Idle"},
		{"BossAttack", "Boss_Attack"},
		{"BossHurt", "Boss_Hurt"},
		{"BossDeath", "Boss_Death" }
	};

	std::string NIKE::State::getSpriteSheet(const std::string& fsm_state, const std::string& element)
	{
		if (fsm_state == "BossDeath")
		{
			return state_to_spritesheet[fsm_state] + "_Spritesheet" + ".png";
		}
		return state_to_spritesheet[fsm_state] + "_" + element + "_Spritesheet" + ".png";
	}

	void State::setBossAnimation(Entity::Type const& entity, const std::string& fsm_state, const std::string& element,
		float dir, int start_x, int end_x)
	{
		// Get corresponding state spritesheet
		std::string boss_spritesheet = getSpriteSheet(fsm_state, element);

		// Get relevant components
		auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
		auto e_ani_sprite_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
		auto e_ani_base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);

		if (e_texture_comp.has_value() && e_ani_sprite_comp.has_value() && e_ani_base_comp.has_value())
		{
			// Determine animation direction
			int anim_direction = 0;
			bool flip_x = false;

			if (dir >= -M_PI / 8 && dir < M_PI / 8) {
				// Front
				anim_direction = 1;
				flip_x = true;
			}
			else if (dir >= M_PI / 8 && dir < 3 * M_PI / 8) {
				// Front-Right
				anim_direction = 1;
			}
			else if (dir >= 3 * M_PI / 8 && dir < 5 * M_PI / 8) {
				// Back-Right
				anim_direction = 3;
			}
			else if (dir >= 5 * M_PI / 8 && dir < 7 * M_PI / 8) {
				// Back
				anim_direction = 2;
			}
			else if (dir >= -3 * M_PI / 8 && dir < -M_PI / 8) {
				// Front-Left (flip it to look right)
				anim_direction = 1;
				flip_x = true;
			}
			else if (dir >= -5 * M_PI / 8 && dir < -3 * M_PI / 8) {
				// Back-Left (flip it to look right)
				anim_direction = 3;
				flip_x = true;
			}
			else {
				// Back
				anim_direction = 1;
			}

			// Flip X when facing left
			//flip_x = (anim_direction == 1 || anim_direction == 3);

			// Change the spritesheet accordingly
			e_texture_comp.value().get().texture_id = boss_spritesheet;

			// Set animation based on direction
			Interaction::animationSet(entity, start_x, anim_direction, end_x, anim_direction);
			
			Interaction::flipX(entity, flip_x);
			// Keeping flipY as false unless needed
			Interaction::flipY(entity, false); 
			Interaction::setLastDirection(entity, anim_direction);
		}
	}


	/*******************************
	* Boss Idle State functions
	*****************************/

	State::BossIdleState::BossIdleState()
	{
		// Add transitions here
		addTransition("BossIdleToBossAttack", std::make_shared<Transition::BossIdleToBossAttack>());
		addTransition("BossIdleToBossChase", std::make_shared<Transition::BossIdleToBossChase>());
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
		//auto e_enemy_dyna = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
		//if (e_enemy_dyna.has_value())
		//{
		//	// Force Stop boss from moving when idle
		//	e_enemy_dyna.value().get().velocity = { 0,0 };
		//}
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
		addTransition("BossAttackToBossChase", std::make_shared<Transition::BossAttackToBossChase>());
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

	void State::BossAttackState::onEvent(std::shared_ptr<Physics::CollisionEvent> event)
	{
		// Ensure entities exist and handle the collision
		if (NIKE_ECS_MANAGER->checkEntity(event->entity_a) && NIKE_ECS_MANAGER->checkEntity(event->entity_b)) {
			Interaction::handleCollision(event->entity_a, event->entity_b);
		}
	}

	/*******************************
	* Boss Chase State functions
	*****************************/

	State::BossChaseState::BossChaseState() : cell_offset{ 15.0f }, boss_speed{ 800.0f }
	{
		// Add transitions here
		addTransition("BossChaseToBossAttack", std::make_shared<Transition::BossIdleToBossAttack>());
		addTransition("BossChaseToBossDeath", std::make_shared<Transition::BossIdleToBossDeath>());
	}

	void State::BossChaseState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;
		//if (!checkTransitionExist("IdleToAttack"))
		//{
		//	addTransition("IdleToAttack", std::make_shared<Transition::IdleToAttack>());
		//}
	}

	void State::BossChaseState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		if (NIKE_METADATA_SERVICE->getEntitiesByTag("player").empty())
		{
			// Stop SFX when walking
			auto e_audio_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
			if (e_audio_comp.has_value())
			{
				playSFX(entity, false);
			}
		}

		for (auto& other_entity : NIKE_METADATA_SERVICE->getEntitiesByTag("player"))
		{
			// Getting components from player and enemy entities
			auto e_player_game_logic = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
			auto e_player_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(other_entity);

			auto e_enemy_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
			auto e_enemy_dyna = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
			auto e_enemy_health = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
			auto e_animation = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);

			if (e_enemy_transform.has_value() && e_player_game_logic.has_value() && e_player_transform.has_value() && e_enemy_dyna.has_value()
				&& e_enemy_health.has_value() && e_animation.has_value())
			{
				auto& player_transform = e_player_transform.value().get();

				// Get enemy position as starting cell
				auto end = NIKE_MAP_SERVICE->getCellIndexFromCords(player_transform.position);
				if (!end.has_value()) return;

				// Move the entity along the computed path
				Enemy::moveAlongPath(entity, end.value().x, end.value().y, boss_speed, cell_offset);
				// Play SFX when walking
				auto e_audio_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
				if (e_audio_comp.has_value())
				{
					playSFX(entity, true);
				}
			}


			// Update animation based on movement direction
			//if (e_enemy_dyna.has_value() && e_enemy_dyna.value().get().force.length() > 0.01f) {
			//	float dir = atan2(e_enemy_dyna.value().get().force.y, e_enemy_dyna.value().get().force.x);
			//	updateEnemyChaseAnimation(entity, dir);
			//}

		}
	}

	void State::BossChaseState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
		// removeTransition("IdleToAttack");
	}

	void State::BossChaseState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
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
		auto texture = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
		auto elem = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(entity);
		if (dynamics.has_value()) {

			dynamics.value().get().max_speed = { 0.0f };
		}

		// Play EnemyDeathState animation
		setBossAnimation(entity, "BossDeath", "", 0, 0, 6);
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
