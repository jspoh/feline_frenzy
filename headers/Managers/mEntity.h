/*****************************************************************//**
 * \file   Entity.h
 * \brief  Entity manager for ecs architecture
 *
 * \author ho
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef M_ENTITY_HPP
#define M_ENTITY_HPP

namespace Entity {

	//Entity Management
	class Manager {
	private:

		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		//Map of create entities and their component signatures
		std::unordered_map<Entity::Type, Component::Signature> entities;

		//Entity indexes waiting to be used
		std::queue<Entity::Type> avail_entities;

	public:

		//Default constructor
		Manager();

		//Create Entity
		Entity::Type createEntity();

		//Destroy Entity
		void destroyEntity(Entity::Type entity);

		//Set signature
		void setSignature(Entity::Type entity, Component::Signature signature);

		Entity::Type cloneEntity(Entity::Type original_entity);

		//Get signature
		Component::Signature const& getSignature(Entity::Type entity) const;

		//Get number of active entities
		int getEntityCount() const;
	};
}

#endif //!M_ENTITY_HPP