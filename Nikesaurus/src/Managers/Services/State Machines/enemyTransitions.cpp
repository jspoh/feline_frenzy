/*****************************************************************//**
 * \file   enemyTransitions.cpp
 * \brief  Enemy transitions
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"

// States and transitions
#include "Managers/Services/State Machine/enemyTransitions.h"
#include "Managers/Services/State Machine/enemyStates.h"
#include "Systems/GameLogic/sysEnemy.h"


namespace NIKE {

	/*******************************
	* Idle To Attack transition functions
	*****************************/

	bool Transition::IdleToAttack::isValid(Entity::Type& entity) const
	{
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& other_entity : NIKE_ECS_MANAGER->getAllEntities()) {
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
			// Somehow e_player_comp is getting nullptr
			if (e_player_comp.has_value())
			{
				// If entity has the gamelogic::ilogic component, and within range of enemy
				if (Enemy::withinRange(entity, other_entity)){
					return true;
				}
			}

		}

		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::IdleToAttack::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::AttackState>("Attack");
	}

	/*******************************
	* Idle To Chase transition functions
	*****************************/

	bool Transition::IdleToChase::isValid(Entity::Type& entity) const
	{
		// This function will check if there is a path from player to enemy entity(param)
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& other_entity : NIKE_ECS_MANAGER->getAllEntities()) {
			// Getting comps
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
			auto e_player_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(other_entity);
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

				// If path exist, transition to chase
				// Check conditions that would trigger transition to Chase state
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
				if (!path.path.empty() && !Enemy::withinRange(entity, other_entity)) {
					return true;
				}
			}

		}
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::IdleToChase::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::ChaseState>("Chase");
	}

	/*******************************
	* Idle To Death transition functions
	*****************************/

	bool Transition::IdleToDeath::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::IdleToDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::DeathState>("Death");
	}

	/*******************************
	* Attack To Idle transition functions
	*****************************/

	bool Transition::AttackToIdle::isValid(Entity::Type& entity) const
	{
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& other_entity : NIKE_ECS_MANAGER->getAllEntities()) {
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
			if (e_player_comp.has_value())
			{
				// If entity has the gamelogic::ilogic component, and not within range of enemy
				if (!Enemy::withinRange(entity, other_entity)) {
					return true;
				}
			}

		}

		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::AttackToIdle::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::IdleState>("Idle");
	}

	/*******************************
	* Attack To Chase transition functions
	*****************************/

	bool Transition::AttackToChase::isValid(Entity::Type& entity) const
	{
		// This function will check if there is a path from player to enemy entity(param)
				// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& other_entity : NIKE_ECS_MANAGER->getAllEntities()) {
			// Getting comps
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
			auto e_player_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(other_entity);
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

				// If path exist, transition to chase
				// Check conditions that would trigger transition to Chase state
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
				if (!path.path.empty() && !Enemy::withinRange(entity, other_entity)) {
					return true;
				}
			}

		}
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::AttackToChase::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::ChaseState>("Chase");
	}

	/*******************************
	* Attack To Death transition functions
	*****************************/

	bool Transition::AttackToDeath::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::AttackToDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::DeathState>("Death");
	}

	/*******************************
	* Chase To Attack transition functions
	*****************************/

	bool Transition::ChaseToAttack::isValid(Entity::Type& entity) const
	{
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& other_entity : NIKE_ECS_MANAGER->getAllEntities()) {
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
			if (e_player_comp.has_value())
			{
				// If entity has the gamelogic::ilogic component, and not within range of enemy
				if (Enemy::withinRange(entity, other_entity)) {
					return true;
				}
			}

		}

		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::ChaseToAttack::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::AttackState>("Attack");
	}

	/*******************************
	* Chase To Idle transition functions
	*****************************/

	bool Transition::ChaseToIdle::isValid(Entity::Type& entity) const
	{
		// This function will check if there is a path from player to enemy entity(param)
		// Look for entity w player component, do like this first, when meta data is out, no need iterate through
		for (auto& other_entity : NIKE_ECS_MANAGER->getAllEntities()) {
			// Getting comps
			auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);

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

	std::shared_ptr<StateMachine::Istate> Transition::ChaseToIdle::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::IdleState>("Idle");
	}

	/*******************************
	* Chase To Death transition functions
	*****************************/

	bool Transition::ChaseToDeath::isValid([[maybe_unused]] Entity::Type& entity) const
	{
		return false;
	}

	std::shared_ptr<StateMachine::Istate> Transition::ChaseToDeath::getNextState() const
	{
		return NIKE_FSM_SERVICE->getStateByID<State::DeathState>("Death");
	}
}
