/*****************************************************************//**
 * \file   sMetaData.cpp
 * \brief  Meta Data Management
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/sMetaData.h"

namespace NIKE {

	void MetaData::Service::onEvent(std::shared_ptr<Coordinator::EntitiesChanged> event) {

		//Get entities
		auto& ecs_entities = event->entities;

		//Remove entities that are no longer in the ECS
		for (auto it = entities.begin(); it != entities.end();) {
			if (ecs_entities.find(it->first) == ecs_entities.end()) {
				it = entities.erase(it);
			}
			else {
				++it;
			}
		}

		//Add new entities from the ECS that are not yet in the editor
		for (auto& entity : ecs_entities) {

			if (entities.find(entity) == entities.end()) {

				//Create identifier for entity
				char entity_name[32];
				snprintf(entity_name, sizeof(entity_name), "entity_%04d", static_cast<int>(entities.size()));

				//Add entity to editor structures
				entities.emplace(entity, EntityData(entity_name, *entity_types.begin(), *entity_categories.begin()));
			}
		}
	}

	void MetaData::Service::addEntityType(std::string const& type) {
		entity_types.insert(type);
	}

	void MetaData::Service::removeEntityType(std::string const& type) {
		entity_types.erase(type);
	}

	bool MetaData::Service::checkEntityType(std::string const& type) const {
		return entity_types.find(type) != entity_types.end();
	}

	std::set<std::string> MetaData::Service::getEntityTypes() const {
		return entity_types;
	}

	void MetaData::Service::addEntityCategory(std::string const& category) {
		entity_types.insert(category);
	}

	void MetaData::Service::removeEntityCategory(std::string const& category) {
		entity_types.erase(category);
	}

	bool MetaData::Service::checkEntityCategory(std::string const& category) const {
		return entity_categories.find(category) != entity_categories.end();
	}

	std::set<std::string> MetaData::Service::getEntityCategories() const {
		return entity_categories;
	}

	bool MetaData::Service::setEntityName(Entity::Type entity, std::string const& name) {
		//Check if name has been taken
		for (auto const& entity_data : entities) {
			if (entity_data.second.name == name) {
				return false;
			}
		}

		//Set name
		entities.at(entity).name = name;
		return true;
	}

	bool MetaData::Service::setEntityType(Entity::Type entity, std::string const& type) {
		//Check if type has been registered with meta data service
		if (entity_types.find(type) == entity_types.end()) {
			return false;
		}

		entities.at(entity).type = type;
		return true;
	}

	bool MetaData::Service::setEntityCategory(Entity::Type entity, std::string const& category) {
		//Check if category has been registered with meta data service
		if (entity_categories.find(category) == entity_categories.end()) {
			return false;
		}

		entities.at(entity).category = category;
		return true;
	}

	MetaData::EntityData MetaData::Service::getEntityData(Entity::Type entity) const {
		return entities.find(entity)->second;
	}

	std::map<Entity::Type, MetaData::EntityData, MetaData::Service::EntitySorter> MetaData::Service::getEntitiesData() const {
		return entities;
	}
}