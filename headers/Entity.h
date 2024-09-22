/*****************************************************************//**
 * \file   Entity.h
 * \brief  Entity manager for ecs architecture
 *
 * \author ho
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef ENTITY_HPP
#define ENTITY_HPP

namespace Entity {

	//Entity Management
	class Manager {
	private:

		//Default constructor
		Manager();

		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		//Map of create entities and their component signatures
		std::unordered_map<Entity::Type, Component::Signature> entities;

		//Entity indexes waiting to be used
		std::queue<Entity::Type> avail_entities;

	public:

		//Singleton Of Manager Class
		static Manager& getInstance() {
			static Manager instance;
			return instance;
		}

		//Create Entity
		Entity::Type createEntity();

		//Destroy Entity
		void destroyEntity(Entity::Type entity);

		//Set signature
		template<typename T>
		void setSignature(Entity::Type entity, T component) {
			entities.at(entity).set(component, true);
		}

		//Get signature
		Component::Signature getSignature(Entity::Type entity) const;

		//Get number of active entities
		int getEntityCount() const;
	};
}


#endif //!ENTITY_HPP