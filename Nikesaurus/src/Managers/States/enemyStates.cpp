/*****************************************************************//**
 * \file   cEnemyStates.cpp
 * \brief  Enemy States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/States/enemyStates.h"
#include "Core/Engine.h"

namespace NIKE {

	/*******************************
	* Idle State functions
	*****************************/

	void State::IdleState::onEnter()
	{
		cout << "enter idle state" << endl;
	}

	void State::IdleState::onUpdate()
	{
		// cout << "update Idle State" << endl;
	}

	void State::IdleState::onExit()
	{
		cout << "exit idle state" << endl;
	}

	bool State::IdleState::withinRange(const Entity::Type& enemy, const Entity::Type& player) {
		// Get player transform
		auto player_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
		Vector2f player_pos = player_transform_comp.value().get().position;
		Vector2f player_scale = player_transform_comp.value().get().scale;

		// Get enemy transform
		auto enemy_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(enemy);
		Vector2f enemy_pos = enemy_transform_comp.value().get().position;
		Vector2f enemy_scale = enemy_transform_comp.value().get().scale;

		// Get enemy range
		auto enemy_attack_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(enemy);
		float enemy_range = enemy_attack_comp.value().get().range;

		// Calculations
		float avg_scale_x = (enemy_scale.x + player_scale.x) / 2;
		float avg_scale_y = (enemy_scale.y + player_scale.y) / 2;

		float dist_x = (enemy_pos.x - player_pos.x) / avg_scale_x;
		float dist_y = (enemy_pos.y - player_pos.y) / avg_scale_y;

		float distance = (dist_x * dist_x) + (dist_y * dist_y);

		//NIKEE_CORE_INFO("Distance = {}, Enemy Range = {}", distance, enemy_range);

		// It is recommended to use enemy_range^2, but it's probably easier this way
		return distance < enemy_range;
	}

	/*******************************
	* Attack State functions
	*****************************/

	void NIKE::State::AttackState::onEnter()
	{
		cout << "enter attack state" << endl;
	}

	void NIKE::State::AttackState::onUpdate()
	{
		// If within range, shoot bullet
		//shootBullet(enemy, player);
		// cout << "update attack state" << endl;
	}

	void NIKE::State::AttackState::onExit()
	{
		cout << "exit attack state" << endl;
	}

	void State::AttackState::shootBullet(const Entity::Type& enemy, const Entity::Type& player) {
		// Get player transform component
		const auto p_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
		const Vector2f& player_pos = p_transform_comp.value().get().position;

		// Get enemy components
		// Transform Comp
		const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(enemy);
		const Vector2f& enemy_pos = e_transform_comp.value().get().position;

		// Attack Comp
		const auto e_attack_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(enemy);
		const auto& enemy_attack_comp = e_attack_comp.value().get();
		//const std::string& bullet_prefab = enemy_attack_comp.prefab_path;

		// Element comp
		const auto e_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(enemy);

		// Create entity for bullet
		// Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity(enemy_attack_comp.layer);
		// I don't think this layer number actually matters, since it depends on the prefab layer
		Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity(1);

		// Load entity from prefab
		if (e_element_comp.has_value()) {
			// Shoot elemental bullet
			NIKE_SERIALIZE_SERVICE->loadEntityFromFile(bullet_entity, NIKE_ASSETS_SERVICE->getAssetPath(Element::enemyBullet[static_cast<int>(e_element_comp.value().get().element)]).string());
		}
		else {
			// Missing Element Comp
			NIKEE_CORE_WARN("ENEMY missing Elemental Component");
			NIKE_SERIALIZE_SERVICE->loadEntityFromFile(bullet_entity, NIKE_ASSETS_SERVICE->getAssetPath("bullet.prefab").string());
		}
		// Calculate direction for bullet (Enemy Pos - Player Pos)
		Vector2f direction = player_pos - enemy_pos;
		direction.normalize();

		// Offset spawn position of bullet
		const float& offset = enemy_attack_comp.offset;
		Vector2f bullet_pos = enemy_pos + (direction * offset);

		// Set bullet's position
		auto bullet_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(bullet_entity);
		if (bullet_transform_comp.has_value()) {
			bullet_transform_comp.value().get().position = bullet_pos;
		}

		// Set bullet physics
		auto bullet_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(bullet_entity);
		if (bullet_physics_comp.has_value()) {
			// Set force
			bullet_physics_comp.value().get().force = { direction.x, direction.y };
		}
	}

	/*******************************
	* Chase State functions
	*****************************/
	void NIKE::State::ChaseState::onEnter()
	{
		cout << "enter chase state" << endl;
	}

	void NIKE::State::ChaseState::onUpdate()
	{
		// cout << "update chase state" << endl;
	}

	void NIKE::State::ChaseState::onExit()
	{
		cout << "exit chase state" << endl;
	}
}
