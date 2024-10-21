/*****************************************************************//**
 * \file   mEntity.cpp
 * \brief  Entity manager for ECS architecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/ECS/mEntity.h"

namespace NIKESAURUS {
	Entity::Manager::Manager() {
		//Fill avail_entities with all possibles indexes up to the limit
		for (Entity::Type i = 0; i < Entity::MAX; i++) {
			avail_entities.push(i);
		}
	}

	Entity::Type Entity::Manager::createEntity() {

		//Check if entity has reached the max limit
		assert(!avail_entities.empty() && "Too many entities created.");

		//Construct a new entity into entities map
		Entity::Type id{ avail_entities.front() };
		entities.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple());
		avail_entities.pop();
		return id;
	}

	Entity::Type Entity::Manager::cloneEntity(Entity::Type original_entity) {

		static_cast<void> (original_entity);
		assert(entities.find(original_entity) != entities.end() && "Entity not found.");
		return 0;
	}

	void Entity::Manager::destroyEntity(Entity::Type entity) {
		//Check if entity has alr been created
		assert(entities.find(entity) != entities.end() && "Entity not found.");

		//Erase entity and push into avail entities
		entities.erase(entity);
		avail_entities.push(entity);
	}

	void Entity::Manager::setSignature(Entity::Type entity, Component::Signature signature) {
		//Check if entity has alr been created
		assert(entities.find(entity) != entities.end() && "Entity not found.");

		//Set Signature
		entities.at(entity) = signature;
	}

	Component::Signature const& Entity::Manager::getSignature(Entity::Type entity) const {
		//Check if entity has alr been created
		assert(entities.find(entity) != entities.end() && "Entity not found.");

		//Get Signature
		return entities.at(entity);
	}

	int Entity::Manager::getEntityCount() const {
		return static_cast<int>(entities.size());
	}

	std::vector<Entity::Type> Entity::Manager::getAllEntities() const {
		std::vector<Entity::Type> return_vec;
		for (auto const& entity : entities) {
			return_vec.push_back(entity.first);
		}
		return return_vec;
	}
}
