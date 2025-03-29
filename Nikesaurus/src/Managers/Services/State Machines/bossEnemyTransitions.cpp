/*****************************************************************//**
 * \file   bossEnemyTransitions.cpp
 * \brief  Boss transitions
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"

 // States and transitions
#include "Managers/Services/State Machine/bossEnemyTransitions.h"
#include "Managers/Services/State Machine/bossEnemyStates.h"
#include "Managers/Services/State Machine/enemyUtils.h"

namespace NIKE {

	/*******************************
	* Boss Idle To Boss Attack transition functions
	*****************************/

	bool Transition::BossIdleToBossAttack::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::Script>(player);
			// Somehow e_player_comp is getting nullptr
			if (e_player_comp.has_value())
			{
				// If entity has the GameLogic::Script component, and within range of enemy
				if (Enemy::isWithinGridRange(entity, player)) {
					return true;
				}
			}

		}

		return false;	
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossIdleToBossAttack::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossAttackState>("BossAttack");
	}

	/*******************************
	* Boss Idle To Boss Chase transition functions
	*****************************/

	bool Transition::BossIdleToBossChase::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		// This function will check if there is a path from player to enemy entity(param)
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
			// Getting comps
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::Script>(player);
			auto e_player_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
			auto e_enemy_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
			auto e_enemy_combat = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity);

			if (e_player_comp.has_value() && e_player_transform.has_value() && e_enemy_transform.has_value() &&
				e_enemy_combat.has_value())
			{
				auto& player_transform = e_player_transform.value().get();
				auto& enemy_transform = e_enemy_transform.value().get();

				// Getting cell indexes from enemy and player pos
				auto start = NIKE_MAP_SERVICE->getCellIndexFromCords(enemy_transform.position);
				auto end = NIKE_MAP_SERVICE->getCellIndexFromCords(player_transform.position);

				// Prevent throw
				if (!start.has_value() || !end.has_value()) {
					// Skip if the indexes are invalid
					continue;
				}

				// If path exist, transition to EnemyChaseState
				// Check conditions that would trigger transition to EnemyChaseState state
				// Find path for player
				if (!NIKE_MAP_SERVICE->checkPath(entity) ||

					//Condition if changes to grid blocked has been made
					NIKE_MAP_SERVICE->checkGridChanged() ||

					//Check if target got shifted
					(NIKE_MAP_SERVICE->getPath(entity).goal.index != end.value()) ||

					//Check if entity got shifted
					(!NIKE_MAP_SERVICE->getPath(entity).path.empty() &&
						(std::abs(NIKE_MAP_SERVICE->getPath(entity).path.front().index.x - start.value().x) > 1 ||
							std::abs(NIKE_MAP_SERVICE->getPath(entity).path.front().index.y - start.value().y) > 1)) ||

					//Check if path is finished & entity got shifted
					(NIKE_MAP_SERVICE->getPath(entity).b_finished && start.value() != NIKE_MAP_SERVICE->getPath(entity).end.index)

					) {
					NIKE_MAP_SERVICE->findPath(entity, start.value(), end.value());
				}

				// Transition happens when path is not empty
				auto path = NIKE_MAP_SERVICE->getPath(entity);
				if (!path.path.empty() && !Enemy::isWithinGridRange(entity, player) && !NIKE_METADATA_SERVICE->getEntitiesByTag("player").empty()) {
					return true;
				}
			}

		}
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossIdleToBossChase::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossChaseState>("BossChase");
	}

	/*******************************
	* Boss Idle To Boss Death transition functions
	*****************************/

	bool Transition::BossIdleToBossDeath::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
		const auto faction_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity);
		if (target_health_comp.has_value() && faction_comp.has_value())
		{
			if (target_health_comp.value().get().lives <= 0) {
				return true;
			}
		}
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossIdleToBossDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossDeathState>("BossDeath");
	}

	/*******************************
	* Boss Attack To Boss Idle transition functions
	*****************************/

	bool Transition::BossAttackToBossIdle::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::Script>(player);
			if (e_player_comp.has_value())
			{
				// If entity has the GameLogic::Script component, and not within range of enemy
				if (!Enemy::isWithinGridRange(entity, player)) {
					return true;
				}
			}

		}

		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossAttackToBossIdle::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossIdleState>("BossIdle");
	}

	/*******************************
	* Boss Attack To Boss Chase transition functions
	*****************************/

	bool Transition::BossAttackToBossChase::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		// This function will check if there is a path from player to enemy entity(param)
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
			// Getting comps
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::Script>(player);
			auto e_player_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
			auto e_enemy_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
			auto e_enemy_combat = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity);

			if (e_player_comp.has_value() && e_player_transform.has_value() && e_enemy_transform.has_value() &&
				e_enemy_combat.has_value())
			{
				auto& player_transform = e_player_transform.value().get();
				auto& enemy_transform = e_enemy_transform.value().get();

				// Getting cell indexes from enemy and player pos
				auto start = NIKE_MAP_SERVICE->getCellIndexFromCords(enemy_transform.position);
				auto end = NIKE_MAP_SERVICE->getCellIndexFromCords(player_transform.position);

				// Prevent throw
				if (!start.has_value() || !end.has_value()) {
					// Skip if the indexes are invalid
					continue;
				}

				// If path exist, transition to EnemyChaseState
				// Check conditions that would trigger transition to EnemyChaseState state
				// Find path for player
				if (!NIKE_MAP_SERVICE->checkPath(entity) ||

					//Condition if changes to grid blocked has been made
					NIKE_MAP_SERVICE->checkGridChanged() ||

					//Check if target got shifted
					(NIKE_MAP_SERVICE->getPath(entity).goal.index != end.value()) ||

					//Check if entity got shifted
					(!NIKE_MAP_SERVICE->getPath(entity).path.empty() &&
						(std::abs(NIKE_MAP_SERVICE->getPath(entity).path.front().index.x - start.value().x) > 1 ||
							std::abs(NIKE_MAP_SERVICE->getPath(entity).path.front().index.y - start.value().y) > 1)) ||

					//Check if path is finished & entity got shifted
					(NIKE_MAP_SERVICE->getPath(entity).b_finished && start.value() != NIKE_MAP_SERVICE->getPath(entity).end.index)

					) {
					NIKE_MAP_SERVICE->findPath(entity, start.value(), end.value());
				}

				// Transition happens when path is not empty
				auto path = NIKE_MAP_SERVICE->getPath(entity);
				if (!path.path.empty() && !Enemy::isWithinGridRange(entity, player) && !NIKE_METADATA_SERVICE->getEntitiesByTag("player").empty()) {
					return true;
				}
			}

		}
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossAttackToBossChase::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossChaseState>("BossChase");
	}

	/*******************************
	* Boss Attack To Boss Death transition functions
	*****************************/

	bool Transition::BossAttackToBossDeath::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
		const auto faction_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity);
		if (target_health_comp.has_value() && faction_comp.has_value())
		{
			if (target_health_comp.value().get().lives <= 0) {
				return true;
			}
		}
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossAttackToBossDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossDeathState>("BossDeath");
	}

	/*******************************
	* Boss Chase To Boss Attack transition functions
	*****************************/

	bool Transition::BossChaseToBossAttack::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::Script>(player);
			// Somehow e_player_comp is getting nullptr
			if (e_player_comp.has_value())
			{
				// If entity has the GameLogic::Script component, and within range of enemy
				if (Enemy::isWithinGridRange(entity, player)) {
					return true;
				}
			}

		}

		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossChaseToBossAttack::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossAttackState>("BossAttack");
	}

	/*******************************
	* Boss Chase To Boss Death transition functions
	*****************************/

	bool Transition::BossChaseToBossDeath::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
		const auto faction_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity);
		if (target_health_comp.has_value() && faction_comp.has_value())
		{
			if (target_health_comp.value().get().lives <= 0) {
				return true;
			}
		}
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::BossChaseToBossDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossDeathState>("BossDeath");
	}
}


