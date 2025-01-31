/*****************************************************************//**
 * \file   sMetaData.h
 * \brief  Meta Data Management
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef M_METADATA_HPP
#define M_METADATA_HPP

#include "Managers/ECS/mCoordinator.h"

namespace NIKE {
	namespace MetaData {

		//Data Type
		struct EntityData {

			//Name
			std::string name;

			//Dynamic tagging
			std::set<std::string> tags;

			//Boolean for checking if entity is active
			bool b_isactive;

			EntityData(std::string const& name) 
				: name{ name }, b_isactive{ true } {}
		};

		//Meta Data Management
		class Service : public Events::IEventListener<Coordinator::EntitiesChanged> {
		private:

			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			//Entities name
			std::set<std::string> entity_names;

			//Entity Types
			std::set<std::string> entity_tags;

			//Sort entities
			struct EntitySorter {
				bool operator()(Entity::Type const& e1, Entity::Type const& e2) const {
					return e1 < e2;
				}
			};

			//Set of active entities
			std::map<Entity::Type, EntityData, EntitySorter> entities;

			//On entities changed event
			void onEvent(std::shared_ptr<Coordinator::EntitiesChanged> event) override;

		public:

			//Default constructor
			Service() = default;

			//Init meta data manager
			void init();

			//Check if type is valid
			bool isTagValid(std::string const& tag);

			//Add new entity type
			void registerTag(std::string const& tag);

			//Remove entity type
			void unregisterTag(std::string const& tag);

			//Get entity types
			std::set<std::string> getRegisteredTag() const;

			//Check Entity Name Taken
			bool isNameValid(std::string const& name) const;

			//Set Entity Name
			bool setEntityName(Entity::Type entity, std::string const& name);

			//Add Entity Tag
			bool addEntityTag(Entity::Type entity, std::string const& tag);

			//Get Entity Tags
			std::set<std::string> getEntityTags(Entity::Type entity);

			//Get entities by type
			std::set<Entity::Type> getEntitiesByTag(std::string const& tag) const;

			//Set Entity State
			void setEntityActive(Entity::Type entity, bool b_active);

			//Clone MetaData except for name
			void cloneEntityData(Entity::Type entity, Entity::Type clone);

			//Get entity data
			EntityData& getEntityData(Entity::Type entity);

			//Get entities data
			std::map<Entity::Type, EntityData, EntitySorter>& getEntitiesData();
		};
	}
}

#endif //!M_ENTITY_HPP