/*****************************************************************//**
 * \file   Entity.cpp
 * \brief  Entity manager for ecs architecture
 *
 * \author ho
 * \date   September 2024
 *********************************************************************/

#include "stdafx.h"
#include "Entity.h"

Entity::Manager::Manager() {
	//Fill avail_entities with all possibles indexes up to the limit
	for (Entity::Type i = 0; i < Entity::MAX; i++) {
		avail_entities.push(i);
	}
}

Entity::Type Entity::Manager::createEntity() {

	//Construct a new entity into entities map
	Entity::Type id{ avail_entities.front() };
	entities.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple());
	avail_entities.pop();
	return id;
}

void Entity::Manager::destroyEntity(Entity::Type entity) {
	entities.erase(entity);
	avail_entities.push(entity);
}

Component::Signature Entity::Manager::getSignature(Entity::Type entity) const {
	return entities.at(entity);
}

int Entity::Manager::getEntityCount() const {
	return static_cast<int>(entities.size());
}