/*****************************************************************//**
 * \file   enemyStates.cpp
 * \brief  Enemy States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/State Machine/enemyStates.h"
#include "Managers/Services/State Machine/enemyUtils.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Core/Engine.h"

// Transitions
#include "Managers/Services/State Machine/enemyTransitions.h"
#include "Managers/Services/State Machine/destructableTransitions.h"

namespace NIKE {

	/*******************************
	* Default State functions
	*****************************/

	State::DefaultState::DefaultState()
	{
		// Add transitions here
		addTransition("DefaultToEnemyIdle", std::make_shared<Transition::DefaultToEnemyIdle>());
		addTransition("DefaultToBossIdle", std::make_shared<Transition::DefaultToBossIdle>());
		addTransition("DefaultToDestructableDeath", std::make_shared<Transition::DefaultToDestructableDeath>());
	}

	void State::DefaultState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;
	}

	void State::DefaultState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update Idle State" << endl;
	}

	void State::DefaultState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
	}

	void State::DefaultState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	{
	}

	/*******************************
	* Idle State functions
	*****************************/

	State::EnemyIdleState::EnemyIdleState()
	{
		// Add transitions here
		addTransition("IdleToAttack", std::make_shared<Transition::IdleToEnemyAttack>());
		addTransition("IdleToEnemyChase", std::make_shared<Transition::IdleToEnemyChase>());
		addTransition("IdleToEnemyDeath", std::make_shared<Transition::IdleToEnemyDeath>());
	}

	void State::EnemyIdleState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;
		//if (!checkTransitionExist("IdleToAttack"))
		//{
		//	addTransition("IdleToAttack", std::make_shared<Transition::IdleToAttack>());
		//}
	}

	void State::EnemyIdleState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update Idle State" << endl;
	}

	void State::EnemyIdleState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
		// removeTransition("IdleToAttack");
	}

	void State::EnemyIdleState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	{
	}

	/*******************************
	* Enemy Attack State functions
	*****************************/

	State::EnemyAttackState::EnemyAttackState()
	{
		// Add transitions here
		addTransition("EnemyAttackToIdle", std::make_shared<Transition::EnemyAttackToIdle>());
		addTransition("EnemyAttackToEnemyChase", std::make_shared<Transition::EnemyAttackToEnemyChase>());
		addTransition("EnemyAttackToEnemyDeath", std::make_shared<Transition::EnemyAttackToEnemyDeath>());
		//addTransition("EnemyAttackToEnemyHurt", std::make_shared<Transition::EnemyAttackToEnemyHurt>());

		// Register the Manager as a listener for collision events
		std::shared_ptr<EnemyAttackState> attack_state_wrapped(this, [](EnemyAttackState*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Physics::CollisionEvent>(attack_state_wrapped);
	}

	void NIKE::State::EnemyAttackState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter attack state" << endl;
		//if (!checkTransitionExist("AttackToIdle"))
		//{
		//	addTransition("AttackToIdle", std::make_shared<Transition::IdleToAttack>());
		//}
	}

	void NIKE::State::EnemyAttackState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
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
			for (auto& other_entity : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
				// Look for entity w player component, do like this first, when meta data is out, no need iterate through
				auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
				// If player entity exists
				if (e_player_comp.has_value()) {
					// Check if player is within range & shot not on cooldown
					if (enemy_comp.last_shot_time >= enemy_comp.cooldown) {
						// Stop enemy when they shooting
						dyna_comp.force = { 0,0 };
						dyna_comp.velocity = { 0,0 };
						// Shoot bullet towards player pos from enemy pos
						Enemy::shootBullet(entity, other_entity);
						float dir = atan2(e_enemy_dyna.value().get().force.y, e_enemy_dyna.value().get().force.x);
						updateEnemyAttackAnimation(entity, dir);
						auto e_audio_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
						if (e_audio_comp.has_value())
						{
							playSFX(entity, false);
						}
						// Reset the last shot time after shooting
						enemy_comp.last_shot_time = 0.f;
					}
				}
			}
		}
		// cout << "update attack state" << endl;
	}

	void NIKE::State::EnemyAttackState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "exit attack state" << endl;
		// removeTransition("AttackToIdle");
	}

	void State::EnemyAttackState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	{
		auto e_sfx_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
		if (e_sfx_comp.has_value()) {
			auto& e_sfx = e_sfx_comp.value().get();

			//Check if group exists
			auto group = NIKE_AUDIO_SERVICE->getChannelGroup(e_sfx.channel_group_id);
			if (!group) {
				e_sfx.b_play_sfx = play_or_no;
				return;
			}
			else {
				//Play sound
				if (play_or_no && !group->isPlaying()) {
					e_sfx.b_play_sfx = play_or_no;
				}
			}

			//stop sfx
			if (!play_or_no) {
				group->stop();
			}
		}
	}

	void State::EnemyAttackState::onEvent(std::shared_ptr<Physics::CollisionEvent> event)
	{
		// Ensure entities exist and handle the collision
		if (NIKE_ECS_MANAGER->checkEntity(event->entity_a) && NIKE_ECS_MANAGER->checkEntity(event->entity_b)) {
			Interaction::handleCollision(event->entity_a, event->entity_b);
		}
	}

	void State::EnemyAttackState::updateEnemyAttackAnimation([[maybe_unused]] Entity::Type& entity, float& dir)
	{

		if (dir >= -M_PI / 8 && dir < M_PI / 8)
		{
			// Attack right
			Interaction::animationSet(entity, 0, 5, 5, 5);
			Interaction::flipX(entity, false);
		}
		else if (dir >= M_PI / 8 && dir < 3 * M_PI / 8)
		{
			// Attack up-right
			Interaction::animationSet(entity, 0, 7, 5, 7);
			Interaction::flipX(entity, false);
		}
		else if (dir >= 3 * M_PI / 8 && dir < 5 * M_PI / 8) {
			// Attack up
			Interaction::animationSet(entity, 0, 7, 5, 7);
			Interaction::flipX(entity, false);
		}
		else if (dir >= 5 * M_PI / 8 && dir < 7 * M_PI / 8) {
			// Attack up-left
			Interaction::animationSet(entity, 0, 7, 5, 7);
			Interaction::flipX(entity, false);
		}
		else if (dir >= -3 * M_PI / 8 && dir < -M_PI / 8) {
			// Attack down-right
			Interaction::animationSet(entity, 0, 4, 5, 4);
			Interaction::flipX(entity, false);
		}
		else if (dir >= -5 * M_PI / 8 && dir < -3 * M_PI / 8) {
			// Attack down
			Interaction::animationSet(entity, 0, 4, 5, 4);
			Interaction::flipX(entity, false);
		}
		else if (dir >= -7 * M_PI / 8 && dir < -5 * M_PI / 8) {
			// Attack up
			Interaction::animationSet(entity, 0, 4, 5, 4);
			Interaction::flipX(entity, false);
		}
		else {
			// Attack left
			Interaction::animationSet(entity, 0, 5, 5, 5);
			Interaction::flipX(entity, true);
		}
	}

	/*******************************
	* Enemy Chase State functions
	*****************************/

	State::EnemyChaseState::EnemyChaseState() : cell_offset{ 15.0f }, enemy_speed{ 1000.0f }
	{
		// Add transitions here
		addTransition("EnemyChaseToIdle", std::make_shared<Transition::EnemyChaseToIdle>());
		addTransition("EnemyChaseToEnemyAttack", std::make_shared<Transition::EnemyChaseToEnemyAttack>());
		//addTransition("EnemyChaseToEnemyHurt", std::make_shared<Transition::EnemyChaseToEnemyHurt>());
		addTransition("EnemyChaseToEnemyDeath", std::make_shared<Transition::EnemyChaseToEnemyDeath>());
	}

	void NIKE::State::EnemyChaseState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter EnemyChaseState state" << endl;
	}

	void State::EnemyChaseState::onUpdate(Entity::Type& entity)
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
				Enemy::moveAlongPath(entity, end.value().x, end.value().y, enemy_speed, cell_offset);
				// Play SFX when walking
				auto e_audio_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
				if (e_audio_comp.has_value())
				{
					playSFX(entity, true);
				}
			}


			// Update animation based on movement direction
			if (e_enemy_dyna.has_value() && e_enemy_dyna.value().get().force.length() > 0.01f) {
				float dir = atan2(e_enemy_dyna.value().get().force.y, e_enemy_dyna.value().get().force.x);
				updateEnemyChaseAnimation(entity, dir);
			}

		}
	}


	void NIKE::State::EnemyChaseState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit EnemyChaseState state" << endl;
	}

	void State::EnemyChaseState::playSFX(Entity::Type& entity, bool play_or_no)
	{
		auto e_sfx_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
		if (e_sfx_comp.has_value()) {
			auto& e_sfx = e_sfx_comp.value().get();

			//Check if group exists
			auto group = NIKE_AUDIO_SERVICE->getChannelGroup(e_sfx.channel_group_id);
			//e_sfx.audio_id = ""

			if (!group) {
				e_sfx.b_play_sfx = play_or_no;
				return;
			}
			else {
				//Play sound
				if (play_or_no && !group->isPlaying()) {
					e_sfx.b_play_sfx = play_or_no;
				}
			}

			//stop sfx
			if (!play_or_no) {
				group->stop();
			}
		}
	}

	void State::EnemyChaseState::updateEnemyChaseAnimation(Entity::Type& entity, float& dir)
	{
		if (dir >= -M_PI / 8 && dir < M_PI / 8) {
			// Moving right
			Interaction::animationSet(entity, 0, 1, 9, 1);
			Interaction::flipX(entity, false);
			Interaction::setLastDirection(entity, 0);
		}
		else if (dir >= M_PI / 8 && dir < 3 * M_PI / 8) {
			// Moving up-right (diagonal)
			Interaction::animationSet(entity, 0, 2, 9, 2);
			Interaction::flipX(entity, false);
			Interaction::setLastDirection(entity, 1);
		}
		else if (dir >= 3 * M_PI / 8 && dir < 5 * M_PI / 8) {
			// Moving up
			Interaction::animationSet(entity, 0, 3, 9, 3);
			Interaction::flipX(entity, false);
			Interaction::setLastDirection(entity, 2);
		}
		else if (dir >= 5 * M_PI / 8 && dir < 7 * M_PI / 8) {
			// Moving up-left (diagonal)
			Interaction::animationSet(entity, 0, 2, 9, 2);
			Interaction::flipX(entity, true);
			Interaction::setLastDirection(entity, 3);
		}
		else if (dir >= -3 * M_PI / 8 && dir < -M_PI / 8) {
			// Moving down-right (diagonal)
			Interaction::animationSet(entity, 0, 1, 9, 1);
			Interaction::flipX(entity, false);
			Interaction::setLastDirection(entity, 4);
		}
		else if (dir >= -5 * M_PI / 8 && dir < -3 * M_PI / 8) {
			// Moving down
			Interaction::animationSet(entity, 0, 0, 9, 0);
			Interaction::flipX(entity, false);
			Interaction::setLastDirection(entity, 5);
		}
		else if (dir >= -7 * M_PI / 8 && dir < -5 * M_PI / 8) {
			// Moving down-left (diagonal)
			Interaction::animationSet(entity, 0, 1, 9, 1);
			Interaction::flipX(entity, true);
			Interaction::setLastDirection(entity, 6);
		}
		else {
			// Moving left
			Interaction::animationSet(entity, 0, 1, 9, 1);
			Interaction::flipX(entity, true);
			Interaction::setLastDirection(entity, 7);
		}
	}

	/*******************************
	* Enemey Death State functions
	*****************************/

	State::EnemyDeathState::EnemyDeathState()
	{
		// Add transitions here
	}

	void State::EnemyDeathState::onEnter([[maybe_unused]] Entity::Type& entity) {
		// Stop enemy from moving
		auto dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
		if (dynamics.has_value()) {

			dynamics.value().get().max_speed = { 0.0f };
		}

		// Play EnemyDeathState animation
		Interaction::animationSet(entity, 0, 8, 10, 8);
		Interaction::flipX(entity, false);
		Interaction::playSFX(entity, true);
	}
	void State::EnemyDeathState::onUpdate([[maybe_unused]] Entity::Type& entity) {

		auto animation_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
		if (animation_comp.has_value())
		{
			if (animation_comp.value().get().completed_animations >= 1) { 
				playSFX(entity, false);
				NIKE_METADATA_SERVICE->destroyEntity(entity);
			}
		}
	}
	void State::EnemyDeathState::onExit([[maybe_unused]] Entity::Type& entity){
		
	}

	void State::EnemyDeathState::playSFX(Entity::Type& entity, bool play_or_no)
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

	/*******************************
	* Enemy Hurt State functions
	*****************************/

	//State::EnemyHurtState::EnemyHurtState()
	//{
	//	// Add transitions here
	//	addTransition("EnemyHurtToEnemyChase", std::make_shared<Transition::EnemyHurtToEnemyChase>());
	//	addTransition("EnemyHurtToEnemyAttack", std::make_shared<Transition::EnemyHurtToEnemyAttack>());
	//	addTransition("EnemyHurtToEnemyDeath", std::make_shared<Transition::EnemyHurtToEnemyDeath>());
	//}

	//void State::EnemyHurtState::onEnter([[maybe_unused]] Entity::Type& entity) {
	//	// Play Enemy Hurt animation
	//	animationSet(entity, 0, 12, 1, 12);
	//	flipX(entity, false);
	//	playSFX(entity, true);
	//}
	//void State::EnemyHurtState::onUpdate([[maybe_unused]] Entity::Type& entity) {
	//	auto animation_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
	//	auto health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
	//	playSFX(entity, true);
	//	if (animation_comp.has_value() && health_comp.has_value())
	//	{
	//		if (animation_comp.value().get().completed_animations >= 1) {
	//			playSFX(entity, false);
	//			NIKE_METADATA_SERVICE->destroyEntity(entity);
	//		}
	//	}
	//}
	//void State::EnemyHurtState::onExit([[maybe_unused]] Entity::Type& entity) {
	//	playSFX(entity, false);
	//}

	//void State::EnemyHurtState::playSFX(Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	//{
	//	Interaction::playOneShotSFX(entity, "EnemyGetHit2.wav", "EnemySFX", 1.0f, 1.0f);
	//}
}


