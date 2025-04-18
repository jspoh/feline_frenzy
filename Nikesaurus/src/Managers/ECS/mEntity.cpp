/*****************************************************************//**
 * \file   mEntity.cpp
 * \brief  Entity manager for ECS architecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/ECS/mEntity.h"


namespace NIKE {
	Entity::Manager::Manager() {
		//Fill avail_entities with all possibles indexes up to the limit
		for (Entity::Type i = 0; i < Entity::MAX; i++) {
			avail_entities.push(i);
		}
	}

	Entity::Type Entity::Manager::createEntity() {

		//Check if entity has reached the max limit
		if (avail_entities.empty()) {
			throw std::runtime_error("Too many entities created.");
		}

		//Construct a new entity into entities map
		Entity::Type id{ avail_entities.front() };
		entities.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple());
		avail_entities.pop();

		return id;
	}

	void Entity::Manager::destroyEntity(Entity::Type entity) {
		//Check if entity has alr been created
		if (entities.find(entity) == entities.end()) {
			//throw std::runtime_error("Entity not found.");
			return;
		}

		//Erase entity and push into avail entities
		entities.erase(entity);
		avail_entities.push(entity);
	}

	bool Entity::Manager::checkEntity(Entity::Type entity) const {
		return entities.find(entity) != entities.end();
	}

	void Entity::Manager::setSignature(Entity::Type entity, Component::Signature signature) {
		//Check if entity has alr been created
		if (entities.find(entity) == entities.end()) {
			throw std::runtime_error("Entity not found.");
		}

		//Set Signature
		entities.at(entity) = signature;
	}

	Component::Signature const& Entity::Manager::getSignature(Entity::Type entity) const {
		//Check if entity has alr been created
		if (entities.find(entity) == entities.end()) {
			throw std::runtime_error("Entity not found.");
		}

		//Get Signature
		return entities.at(entity);
	}

	int Entity::Manager::getEntityComponentCount(Entity::Type entity) const {
		//Check if entity has alr been created
		if (entities.find(entity) == entities.end()) {
			throw std::runtime_error("Entity not found.");
		}

		return static_cast<int>(entities.at(entity).count());
	}

	int Entity::Manager::getEntitiesCount() const {
		return static_cast<int>(entities.size());
	}

	std::set<Entity::Type> Entity::Manager::getAllEntities() const {
		std::set<Entity::Type> return_vec;
		for (auto const& entity : entities) {
			return_vec.insert(entity.first);
		}
		return return_vec;
	}
}
