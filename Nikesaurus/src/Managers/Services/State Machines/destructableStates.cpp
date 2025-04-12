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
		// Somehow this fixes barrel bug?
		for (auto& tag : NIKE_METADATA_SERVICE->getEntityTags(entity))
		{
			if (tag == "objects")
			{
				spawnHealthDrop(entity);
				if (NIKE_SCENES_SERVICE->getCurrSceneID() == "lvl1_1.scn" || NIKE_SCENES_SERVICE->getCurrSceneID() == "lvl1_2.scn") {
					spawnBrokenBarrel(entity);
				}
				else {
					spawnBrokenChest(entity);
				}

				NIKE_AUDIO_SERVICE->playAudio("BreakSFX1.wav", "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), 1.f, false, false);
				NIKE_METADATA_SERVICE->destroyEntity(entity);
			}
			else {
				// Continue iteration when tag is not objects
				continue;
			}

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
			NIKEE_CORE_WARN("spawnHealthDrop: Entity missing Transform Component, health not spawned");
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

	void State::DestructableDeathState::spawnBrokenBarrel(Entity::Type entity) {
		// Get entity position
		const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
		if (!e_transform_comp.has_value()) {
			NIKEE_CORE_WARN("spawnBrokenBarrel: Entity missing Transform Component, barrel not spawned");
			return;
		}

		// Create broken barrel entity
		Entity::Type barrel_entity = NIKE_ECS_MANAGER->createEntity();

		// Load entity from prefab
		NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(barrel_entity, "brokenBarrel.prefab");

		// Set health drop location
		auto drop_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(barrel_entity);
		if (drop_transform_comp.has_value()) {
			drop_transform_comp.value().get().position = e_transform_comp.value().get().position;
		}
	}

	void State::DestructableDeathState::spawnBrokenChest(Entity::Type entity) {
		// Get entity position
		const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
		if (!e_transform_comp.has_value()) {
			NIKEE_CORE_WARN("spawnBrokenBarrel: Entity missing Transform Component, barrel not spawned");
			return;
		}

		// Create broken barrel entity
		Entity::Type barrel_entity = NIKE_ECS_MANAGER->createEntity();

		// Load entity from prefab
		NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(barrel_entity, "brokenChest.prefab");

		// Set health drop location
		auto drop_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(barrel_entity);
		if (drop_transform_comp.has_value()) {
			drop_transform_comp.value().get().position = e_transform_comp.value().get().position;
		}
	}

	
}