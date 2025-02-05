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
	* Idle State functions
	*****************************/

	State::IdleState::IdleState()
	{
		// Add transitions here
		addTransition("IdleToAttack", std::make_shared<Transition::IdleToEnemyAttack>());
		addTransition("IdleToEnemyChase", std::make_shared<Transition::IdleToEnemyChase>());
		addTransition("IdleToEnemyDeath", std::make_shared<Transition::IdleToEnemyDeath>());
		addTransition("IdleToDestructableDeath", std::make_shared<Transition::IdleToDestructableDeathState>());
	}

	void State::IdleState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter idle state" << endl;
		//if (!checkTransitionExist("IdleToAttack"))
		//{
		//	addTransition("IdleToAttack", std::make_shared<Transition::IdleToAttack>());
		//}
	}

	void State::IdleState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		// cout << "update Idle State" << endl;
	}

	void State::IdleState::onExit([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "exit idle state" << endl;
		// removeTransition("IdleToAttack");
	}

	void State::IdleState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
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
			auto& health_comp = e_enemy_health.value().get();
			auto& ani_comp = e_enemy_ani.value().get();

			//if (health_comp.taken_damage) {
			//	if (ani_comp.animation_mode != Animation::Mode::END) {
			//		// Hurt animation still playing
			//		updateEnemyHurtAnimation(entity);
			//		cout << "play hurt animation from attack" << endl;
			//		return;
			//	}
			//	else {
			//		// Animation finished, reset taken_damage
			//		health_comp.taken_damage = false;
			//	}
			//}

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
						updateEnemyAttackAnimation(entity);
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

	void State::EnemyAttackState::updateEnemyAttackAnimation([[maybe_unused]] Entity::Type& entity)
	{
		int get_last_direction = getLastDirection(entity);
		if (get_last_direction == 0)
		{
			// Attack right
			animationStart(entity, 0, 5);
			animationEnd(entity, 5, 5);
			flipX(entity, false);
		}
		else if (get_last_direction == 2) {
			// Attack up
			animationStart(entity, 0, 7);
			animationEnd(entity, 5, 7);
			flipX(entity, false);
		}
		else if (get_last_direction == 5) {
			// Attack down
			animationStart(entity, 0, 4);
			animationEnd(entity, 5, 4);
			flipX(entity, false);
		}
		else{
			// Attack left
			animationStart(entity, 0, 4);
			animationEnd(entity, 5, 4);
			flipX(entity, true);
		}
	}

	void State::EnemyAttackState::updateEnemyHurtAnimation(Entity::Type& entity)
	{
		int get_last_direction = getLastDirection(entity);
		auto animation_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);

		if (animation_comp.has_value()) {
			switch (get_last_direction) {
			case 0:  // Right
				animationStart(entity, 0, 13);
				animationEnd(entity, 1, 13);
				flipX(entity, false);
				break;
			case 2:  // Up
				animationStart(entity, 0, 15);
				animationEnd(entity, 1, 15);
				flipX(entity, false);
				break;
			case 5:  // Down
				animationStart(entity, 0, 12);
				animationEnd(entity, 1, 12);
				flipX(entity, false);
				break;
			default: // Left
				animationStart(entity, 0, 12);
				animationEnd(entity, 1, 12);
				flipX(entity, true);
				break;
			}
		}
	}

	/*******************************
	* Enemy Chase State functions
	*****************************/

	State::EnemyChaseState::EnemyChaseState() : cell_offset{ 15.0f }, enemy_speed{ 1000.0f }
	{
		// Add transitions here
		addTransition("EnemyChaseToEnemyAttack", std::make_shared<Transition::EnemyChaseToEnemyAttack>());
		addTransition("EnemyChaseToIdle", std::make_shared<Transition::EnemyChaseToIdle>());
		addTransition("EnemyChaseToEnemyDeath", std::make_shared<Transition::EnemyChaseToEnemyDeath>());
	}

	void NIKE::State::EnemyChaseState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		//cout << "enter EnemyChaseState state" << endl;
	}

	void State::EnemyChaseState::onUpdate(Entity::Type& entity)
	{
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
				auto& health = e_enemy_health.value().get();

				//if (health.taken_damage) {
				//	if (e_animation.value().get().animation_mode != Animation::Mode::END) {
				//		// Hurt animation still playing
				//		updateEnemyHurtAnimation(entity);  
				//		cout << "play hurt animation from chase" << endl;
				//		return;
				//	}
				//	else {
				//		// Animation finished, reset taken_damage
				//		health.taken_damage = false;  
				//	}
				//}

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
			animationStart(entity, 0, 1);
			animationEnd(entity, 9, 1);
			flipX(entity, false);
			setLastDirection(entity, 0);
		}
		else if (dir >= M_PI / 8 && dir < 3 * M_PI / 8) {
			// Moving up-right (diagonal)
			animationStart(entity, 0, 2);
			animationEnd(entity, 9, 2);
			flipX(entity, false);
			setLastDirection(entity, 1);
		}
		else if (dir >= 3 * M_PI / 8 && dir < 5 * M_PI / 8) {
			// Moving up
			animationStart(entity, 0, 3);
			animationEnd(entity, 9, 3);
			flipX(entity, false);
			setLastDirection(entity, 2);
		}
		else if (dir >= 5 * M_PI / 8 && dir < 7 * M_PI / 8) {
			// Moving up-left (diagonal)
			animationStart(entity, 0, 2);
			animationEnd(entity, 9, 2);
			flipX(entity, true);
			setLastDirection(entity, 3);
		}
		else if (dir >= -3 * M_PI / 8 && dir < -M_PI / 8) {
			// Moving down-right (diagonal)
			animationStart(entity, 0, 1);
			animationEnd(entity, 9, 1);
			flipX(entity, false);
			setLastDirection(entity, 4);
		}
		else if (dir >= -5 * M_PI / 8 && dir < -3 * M_PI / 8) {
			// Moving down
			animationStart(entity, 0, 0);
			animationEnd(entity, 9, 0);
			flipX(entity, false);
			setLastDirection(entity, 5);
		}
		else if (dir >= -7 * M_PI / 8 && dir < -5 * M_PI / 8) {
			// Moving down-left (diagonal)
			animationStart(entity, 0, 1);
			animationEnd(entity, 9, 1);
			flipX(entity, true);
			setLastDirection(entity, 6);
		}
		else {
			// Moving left
			animationStart(entity, 0, 1);
			animationEnd(entity, 9, 1);
			flipX(entity, true);
			setLastDirection(entity, 7);
		}
	}

	void State::EnemyChaseState::updateEnemyHurtAnimation(Entity::Type& entity)
	{
		int get_last_direction = getLastDirection(entity);
		auto animation_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);

		if (animation_comp.has_value()) {
			switch (get_last_direction) {
			case 0:  // Right
				animationStart(entity, 0, 13);
				animationEnd(entity, 1, 13);
				flipX(entity, false);
				break;
			case 2:  // Up
				animationStart(entity, 0, 15);
				animationEnd(entity, 1, 15);
				flipX(entity, false);
				break;
			case 5:  // Down
				animationStart(entity, 0, 12);
				animationEnd(entity, 1, 12);
				flipX(entity, false);
				break;
			default: // Left
				animationStart(entity, 0, 12);
				animationEnd(entity, 1, 12);
				flipX(entity, true);
				break;
			}
		}


	}

	/*******************************
	* EnemyDeathState State functions
	*****************************/

	State::EnemyDeathState::EnemyDeathState()
	{
		// Add transitions here
	}

	void State::EnemyDeathState::onEnter([[maybe_unused]] Entity::Type& entity) {
		// Play EnemyDeathState animation
		animationStart(entity, 0, 8);
		animationEnd(entity, 2, 8);
		flipX(entity, false);
	}
	void State::EnemyDeathState::onUpdate([[maybe_unused]] Entity::Type& entity) {
		auto animation_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
		if (animation_comp.has_value())
		{
			// Use delta time to let animation play before deleting entity
			static float dt = 0.0f;
			dt += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
			if (dt >= 0.2f) { 
				NIKE_METADATA_SERVICE->destroyEntity(entity);
				// Reset delta time
				dt = 0.f;
			}
		}
	}
	void State::EnemyDeathState::onExit([[maybe_unused]] Entity::Type& entity){
		
	}

	void State::EnemyDeathState::playSFX(Entity::Type& entity, bool play_or_no)
	{
		auto e_sfx_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
		if (e_sfx_comp.has_value()) {
			auto& e_sfx = e_sfx_comp.value().get();

			//Check if group exists
			auto group = NIKE_AUDIO_SERVICE->getChannelGroup(e_sfx.channel_group_id);
			e_sfx.audio_id = "EnemyEnemyDeathStateSound2.wav";
			
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
}


