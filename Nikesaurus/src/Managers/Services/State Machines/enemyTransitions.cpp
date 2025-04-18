/*****************************************************************//**
 * \file   enemyTransitions.cpp
 * \brief  Enemy transitions
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"

// States and transitions
#include "Managers/Services/State Machine/enemyTransitions.h"
#include "Managers/Services/State Machine/enemyStates.h"
#include "Managers/Services/State Machine/bossEnemyStates.h"
#include "Managers/Services/State Machine/enemyUtils.h"


namespace NIKE {

	/*******************************
	* Default To Enemy Idle transition functions
	*****************************/

	bool NIKE::Transition::DefaultToEnemyIdle::isValid(Entity::Type& entity) const
	{
		const auto& entity_tags = NIKE_METADATA_SERVICE->getEntityTags(entity);

		// Here should find enemy only and not boss
		if (entity_tags.find("enemy") != entity_tags.end() && entity_tags.find("boss") == entity_tags.end())
		{
			return true;
		}
		return false;
	}


	std::shared_ptr<StateMachine::Istate> Transition::DefaultToEnemyIdle::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::EnemyIdleState>("EnemyIdle");
	}

	/*******************************
	* Default To Boss Idle transition functions
	*****************************/

	bool NIKE::Transition::DefaultToBossIdle::isValid(Entity::Type& entity) const
	{
		const auto& entity_tags = NIKE_METADATA_SERVICE->getEntityTags(entity);

		// Find both boss and enemy tag, boss will have enemy tag to register for the game win overlay
		if (entity_tags.find("enemy") != entity_tags.end() && entity_tags.find("boss") != entity_tags.end())
		{
			return true;
		}

		return false;
	}


	std::shared_ptr<StateMachine::Istate> Transition::DefaultToBossIdle::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::BossIdleState>("BossIdle");
	}



	/*******************************
	* Idle To Enemy Attack transition functions
	*****************************/

	bool Transition::IdleToEnemyAttack::isValid(Entity::Type& entity) const
	{
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(player);
			// Somehow e_player_comp is getting nullptr
			if (e_player_comp.has_value())
			{
				// If entity has the gamelogic::ilogic component, and within range of enemy
				if (Enemy::isWithinGridRange(entity, player)){
					return true;
				}
			}

		}

		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::IdleToEnemyAttack::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::EnemyAttackState>("EnemyAttack");
	}

	/*******************************
	* Idle To Enemy Chase State transition functions
	*****************************/

	bool Transition::IdleToEnemyChase::isValid(Entity::Type& entity) const
	{
		// This function will check if there is a path from player to enemy entity(param)
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
			// Getting comps
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(player);
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

	std::shared_ptr<StateMachine::Istate> Transition::IdleToEnemyChase::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::EnemyChaseState>("EnemyChase");
	}

	/*******************************
	* Idle To Enemy Death State transition functions
	*****************************/

	bool Transition::IdleToEnemyDeath::isValid([[maybe_unused]] Entity::Type& entity) const
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

	std::shared_ptr<StateMachine::Istate> Transition::IdleToEnemyDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::EnemyDeathState>("EnemyDeath");
	}

	/*******************************
	* Enemy Attack To Idle transition functions
	*****************************/

	bool Transition::EnemyAttackToIdle::isValid(Entity::Type& entity) const
	{
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(player);
			if (e_player_comp.has_value())
			{
				// If entity has the gamelogic::ilogic component, and not within range of enemy
				if (!Enemy::isWithinGridRange(entity, player)) {
					return true;
				}
			}

		}

		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::EnemyAttackToIdle::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::EnemyIdleState>("EnemyIdle");
	}

	/*******************************
	* Enemy Attack To Enemy Chase State transition functions
	*****************************/

	bool Transition::EnemyAttackToEnemyChase::isValid(Entity::Type& entity) const
	{
		// This function will check if there is a path from player to enemy entity(param)
		for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
			// Getting comps
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(player);
			auto e_player_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
			auto e_enemy_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);

			if (e_player_comp.has_value() && e_player_transform.has_value() && e_enemy_transform.has_value())
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
				if (!path.path.empty() && !Enemy::isWithinGridRange(entity, player)) {
					return true;
				}
			}

		}
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::EnemyAttackToEnemyChase::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::EnemyChaseState>("EnemyChase");
	}

	/*******************************
	* Attack To Enemy Death State transition functions
	*****************************/

	bool Transition::EnemyAttackToEnemyDeath::isValid([[maybe_unused]] Entity::Type& entity) const
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

	std::shared_ptr<StateMachine::Istate> Transition::EnemyAttackToEnemyDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::EnemyDeathState>("EnemyDeath");
	}

	/*******************************
	* Enemy Chase State To Attack transition functions
	*****************************/

	bool Transition::EnemyChaseToEnemyAttack::isValid(Entity::Type& entity) const
	{
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& other_entity : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
			if (e_player_comp.has_value())
			{
				// If entity has the gamelogic::ilogic component, and not within range of enemy
				if (Enemy::isWithinGridRange(entity, other_entity)) {
					return true;
				}
			}

		}

		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::EnemyChaseToEnemyAttack::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::EnemyAttackState>("EnemyAttack");
	}

	/*******************************
	* Enemy Chase State To Idle transition functions
	*****************************/

	bool Transition::EnemyChaseToIdle::isValid(Entity::Type& entity) const
	{
		// This function will check if there is a path from player to enemy entity(param)
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
			// Getting comps
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(player);

			if (e_player_comp.has_value())
			{
				// Transition happens when path is not empty
				auto path = NIKE_MAP_SERVICE->getPath(entity);
				if (path.path.empty()) {
					return true;
				}
			}

		}
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::EnemyChaseToIdle::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::EnemyIdleState>("EnemyIdle");
	}

	/*******************************
	* Enemy Chase State To Enemy Death State transition functions
	*****************************/

	bool Transition::EnemyChaseToEnemyDeath::isValid([[maybe_unused]] Entity::Type& entity) const
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

	std::shared_ptr<StateMachine::Istate> Transition::EnemyChaseToEnemyDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::EnemyDeathState>("EnemyDeath");
	}

	/*******************************
	* Enemy Attack State To Enemy Hurt State transition functions
	*****************************/

	//bool Transition::EnemyAttackToEnemyHurt::isValid(Entity::Type& entity) const
	//{
	//	const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
	//	const auto faction_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity);
	//	if (target_health_comp.has_value() && faction_comp.has_value())
	//	{
	//		if (target_health_comp.value().get().taken_damage) {
	//			return true;
	//		}
	//	}
	//	return false;
	//}

	//std::shared_ptr<StateMachine::Istate> Transition::EnemyAttackToEnemyHurt::getNextState() const
	//{
	//	return NIKE_FSM_SERVICE->getStateByID<State::EnemyHurtState>("EnemyHurt");
	//}

	/*******************************
	* Enemy Chase State To Enemy Hurt State transition functions
	*****************************/

	//bool Transition::EnemyChaseToEnemyHurt::isValid(Entity::Type& entity) const
	//{
	//	const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
	//	const auto faction_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity);
	//	if (target_health_comp.has_value() && faction_comp.has_value())
	//	{
	//		if (target_health_comp.value().get().taken_damage) {
	//			return true;
	//		}
	//	}
	//	return false;
	//}

	//std::shared_ptr<StateMachine::Istate> Transition::EnemyChaseToEnemyHurt::getNextState() const
	//{
	//	return NIKE_FSM_SERVICE->getStateByID<State::EnemyHurtState>("EnemyHurt");
	//}

	/*******************************
	* Enemy Hurt State To Enemy Attack State transition functions
	*****************************/

	//bool Transition::EnemyHurtToEnemyAttack::isValid(Entity::Type& entity) const
	//{
	//	// Look for entity w player component, do like this first, when meta data is out, no need iterate through
	//	for (auto& other_entity : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
	//		auto health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
	//		auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
	//		if (e_player_comp.has_value() && health_comp.has_value())
	//		{
	//			// If entity has the gamelogic::ilogic component, and not within range of enemy
	//			if (Enemy::isWithinGridRange(entity, other_entity) && !health_comp.value().get().taken_damage) {
	//				return true;
	//			}
	//		}

	//	}
	//	return false;
	//}

	//std::shared_ptr<StateMachine::Istate> Transition::EnemyHurtToEnemyAttack::getNextState() const
	//{
	//	return NIKE_FSM_SERVICE->getStateByID<State::EnemyAttackState>("EnemyAttack");
	//}

	///*******************************
	//* Enemy Hurt State To Enemy Chase State transition functions
	//*****************************/

	//bool Transition::EnemyHurtToEnemyChase::isValid(Entity::Type& entity) const
	//{
	//	// This function will check if there is a path from player to enemy entity(param)
	//	for (auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
	//		// Getting comps
	//		auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(player);
	//		auto e_player_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
	//		auto e_enemy_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
	//		auto health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);

	//		if (e_player_comp.has_value() && e_player_transform.has_value() && e_enemy_transform.has_value()
	//			&& health_comp.has_value())
	//		{
	//			auto& player_transform = e_player_transform.value().get();
	//			auto& enemy_transform = e_enemy_transform.value().get();

	//			// Getting cell indexes from enemy and player pos
	//			auto start = NIKE_MAP_SERVICE->getCellIndexFromCords(enemy_transform.position);
	//			auto end = NIKE_MAP_SERVICE->getCellIndexFromCords(player_transform.position);

	//			// Prevent throw
	//			if (!start.has_value() || !end.has_value()) {
	//				// Skip if the indexes are invalid
	//				continue;
	//			}

	//			// If path exist, transition to EnemyChaseState
	//			// Check conditions that would trigger transition to EnemyChaseState state
	//			// Find path for player
	//			if (!NIKE_MAP_SERVICE->checkPath(entity) ||

	//				//Condition if changes to grid blocked has been made
	//				NIKE_MAP_SERVICE->checkGridChanged() ||

	//				//Check if target got shifted
	//				(NIKE_MAP_SERVICE->getPath(entity).goal.index != end.value()) ||

	//				//Check if entity got shifted
	//				(!NIKE_MAP_SERVICE->getPath(entity).path.empty() &&
	//					(std::abs(NIKE_MAP_SERVICE->getPath(entity).path.front().index.x - start.value().x) > 1 ||
	//						std::abs(NIKE_MAP_SERVICE->getPath(entity).path.front().index.y - start.value().y) > 1)) ||

	//				//Check if path is finished & entity got shifted
	//				(NIKE_MAP_SERVICE->getPath(entity).b_finished && start.value() != NIKE_MAP_SERVICE->getPath(entity).end.index)

	//				) {
	//				NIKE_MAP_SERVICE->findPath(entity, start.value(), end.value());
	//			}

	//			// Transition happens when path is not empty
	//			auto path = NIKE_MAP_SERVICE->getPath(entity);
	//			if (!path.path.empty() && !Enemy::isWithinGridRange(entity, player) && !health_comp.value().get().taken_damage) {
	//				return true;
	//			}
	//		}

	//	}
	//	return false;
	//}

	//std::shared_ptr<StateMachine::Istate> Transition::EnemyHurtToEnemyChase::getNextState() const
	//{
	//	return NIKE_FSM_SERVICE->getStateByID<State::EnemyChaseState>("EnemyChase");
	//}

	/*******************************
	* Enemy Hurt State To Enemy Death State transition functions
	*****************************/

	//bool Transition::EnemyHurtToEnemyDeath::isValid(Entity::Type& entity) const
	//{
	//	// Look for entity w player component, do like this first, when meta data is out, no need iterate through
	//	const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
	//	const auto faction_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity);
	//	if (target_health_comp.has_value() && faction_comp.has_value())
	//	{
	//		if (target_health_comp.value().get().lives <= 0) {
	//			return true;
	//		}
	//	}
	//	return false;
	//}

	//std::shared_ptr<StateMachine::Istate> Transition::EnemyHurtToEnemyDeath::getNextState() const
	//{
	//	return NIKE_FSM_SERVICE->getStateByID<State::EnemyDeathState>("EnemyDeath");
	//}
}
