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
			std::string name;
			std::string type;
			std::string category;

			EntityData(std::string const& name, std::string const& type, std::string const& category) 
				: name{ name }, type{ type }, category{ category } {}
		};

		//Meta Data Management
		class Service : public Events::IEventListener<Coordinator::EntitiesChanged> {
		private:

			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			//Entity Types
			std::set<std::string> entity_types;

			//Entity Categories
			std::set<std::string> entity_categories;

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
			Service();

			//Add new entity type
			void addEntityType(std::string const& type);

			//Remove entity type
			void removeEntityType(std::string const& type);

			//Check entity type
			bool checkEntityType(std::string const& type) const;

			//Get entity types
			std::set<std::string> getEntityTypes() const;

			//Add new entity category
			void addEntityCategory(std::string const& category);

			//Remove entity category
			void removeEntityCategory(std::string const& category);

			//Check entity category
			bool checkEntityCategory(std::string const& type) const;

			//Get entity categories
			std::set<std::string> getEntityCategories() const;

			//Modify Entity Name
			bool setEntityName(Entity::Type entity, std::string const& name);

			//Modify Entity Type
			bool setEntityType(Entity::Type entity, std::string const& type);

			//Modify Entity Category
			bool setEntityCategory(Entity::Type entity, std::string const& category);

			//Get entity data
			EntityData getEntityData(Entity::Type entity) const;

			//Get entities data
			std::map<Entity::Type, EntityData, EntitySorter> getEntitiesData() const;
		};
	}
}

#endif //!M_ENTITY_HPP