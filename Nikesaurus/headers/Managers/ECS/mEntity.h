/*****************************************************************//**
 * \file   mEntity.h
 * \brief  Entity manager for ECS architecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef M_ENTITY_HPP
#define M_ENTITY_HPP

namespace NIKE {
	namespace Entity {

		//Entity Management
		class Manager {
		private:

			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

			//Map of create entities and their component signatures & layer id
			std::unordered_map<Entity::Type, std::pair<Component::Signature, unsigned int>> entities;

			//Entity indexes waiting to be used
			std::queue<Entity::Type> avail_entities;

		public:

			//Default constructor
			Manager();

			//Create Entity
			Entity::Type createEntity(unsigned int layer_id = 0);

			//Destroy Entity
			void destroyEntity(Entity::Type entity);

			//Check entity is present
			bool checkEntity(Entity::Type entity) const;

			//Set signature
			void setSignature(Entity::Type entity, Component::Signature signature);

			//Get signature
			Component::Signature const& getSignature(Entity::Type entity) const;

			//Set layer id
			void setLayerID(Entity::Type entity, unsigned int layer_id);

			//Get layer id
			unsigned int getLayerID(Entity::Type entity) const;

			//Get entity component count
			int getEntityComponentCount(Entity::Type entity) const;

			//Get number of active entities
			int getEntitiesCount() const;

			//Get all active entity
			std::set<Entity::Type> getAllEntities() const;
		};
	}
}

#endif //!M_ENTITY_HPP