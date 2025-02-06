/*****************************************************************//**
 * \file   destructableStates.cpp
 * \brief  Destructable States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/State Machine/enemyUtils.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Core/Engine.h"
#include "Managers/Services/State Machine/destructableStates.h"

namespace NIKE {
	State::DestructableDeathState::DestructableDeathState()
	{
		// Add transitions here
	}

	void State::DestructableDeathState::onEnter([[maybe_unused]] Entity::Type& entity)
	{
		// !TODO: Check for health drop tag

	}

	void State::DestructableDeathState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		auto animation_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
		if (animation_comp.has_value())
		{
			// Use delta time to let animation play before deleting entity
			//static float dt = 0.0f;
			//dt += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
			//if (dt >= 0.2f) {
				spawnHealthDrop(entity);
				NIKE_METADATA_SERVICE->destroyEntity(entity);
				// Reset delta time
				//dt = 0.f;
			//}
		}
	}

	void State::DestructableDeathState::onExit([[maybe_unused]] Entity::Type& entity)
	{

	}

	void State::DestructableDeathState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	{

	}

	void State::DestructableDeathState::spawnHealthDrop(Entity::Type entity) {
		// Get entity position
		const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
		if (!e_transform_comp.has_value()) {
			NIKEE_CORE_WARN("spawnHealthDrop: Entity missing Transform Component, enemy not spawned");
			return;
		}

		// Create health drop entity
		Entity::Type drop_entity = NIKE_ECS_MANAGER->createEntity();

		// Load entity from prefab
		NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(drop_entity, "healthDrop.prefab");

		// Set health drop location
		auto drop_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(drop_entity);
		if (drop_transform_comp.has_value()) {
			drop_transform_comp.value().get().position = e_transform_comp.value().get().position;
		}
	}
}