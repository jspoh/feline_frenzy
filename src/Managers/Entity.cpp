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

	//Check if entity has reached the max limit
	assert(!avail_entities.empty() && "Too many entities created.");

	//Construct a new entity into entities map
	Entity::Type id{ avail_entities.front() };
	entities.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple());
	avail_entities.pop();
	return id;
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