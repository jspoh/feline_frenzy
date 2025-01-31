/*****************************************************************//**
 * \file   sMetaData.cpp
 * \brief  Meta Data Management
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
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
		int index = 0;
		for (auto& entity : ecs_entities) {

			//Update entities ref
			if (entities.at(entity).name.find("entity_") != std::string::npos) {

				//Create identifier for entity
				char entity_name[32];
				snprintf(entity_name, sizeof(entity_name), "entity_%04d", index++);
				entities.at(entity).name = entity_name;
			}
			else if(entities.find(entity) == entities.end()) {

				//Create identifier for entity
				char entity_name[32];
				snprintf(entity_name, sizeof(entity_name), "entity_%04d", index++);

				//Add entity to editor structures
				entities.emplace(entity, EntityData(entity_name));
			}
		}
	}

	void MetaData::Service::init() {

		//Setup events listening
		std::shared_ptr<MetaData::Service> metadata_service(this, [](MetaData::Service*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Coordinator::EntitiesChanged>(metadata_service);
	}

	bool MetaData::Service::isTagValid(std::string const& tag) {
		return entity_tags.find(tag) != entity_tags.end();
	}

	void MetaData::Service::registerTag(std::string const& tag) {
		entity_tags.insert(tag);
	}

	void MetaData::Service::unregisterTag(std::string const& tag) {
		entity_tags.erase(tag);
	}

	std::set<std::string> MetaData::Service::getRegisteredTag() const {
		return entity_tags;
	}

	bool MetaData::Service::isNameValid(std::string const& name) const {
		return entity_names.find(name) == entity_names.end();
	}

	bool MetaData::Service::setEntityName(Entity::Type entity, std::string const& name) {
		//Check if name has been taken
		if (!isNameValid(name)) {
			return false;
		}

		//Set name
		entities.at(entity).name = name;
		return true;
	}

	bool MetaData::Service::addEntityTag(Entity::Type entity, std::string const& tag) {
		//Check if tag has been registered
		if (!isTagValid(tag)) {
			return false;
		}

		//Set tag
		entities.at(entity).tags.insert(tag);
		return true;
	}

	void MetaData::Service::setEntityActive(Entity::Type entity, bool b_active) {
		entities.at(entity).b_isactive = b_active;
	}

	std::set<std::string> MetaData::Service::getEntityTags(Entity::Type entity) {
		return entities.at(entity).tags;
	}

	std::set<Entity::Type> MetaData::Service::getEntitiesByTag(std::string const& tag) const {

		//Set of entity
		std::set<Entity::Type> type_entities;

		//Check if tag is present within entity
		for (auto const& entity_data : entities) {
			if (entity_data.second.tags.find(tag) != entity_data.second.tags.end()) {
				type_entities.insert(entity_data.first);
			}
		}

		return type_entities;
	}

	void MetaData::Service::cloneEntityData(Entity::Type entity, Entity::Type clone) {
		auto it_clone = entities.find(clone);

		if (it_clone == entities.end()) {
			return;
		}

		//Update with cloned meta data
		entities.at(entity).tags = it_clone->second.tags;
		entities.at(entity).b_isactive = it_clone->second.b_isactive;
	}

	MetaData::EntityData& MetaData::Service::getEntityData(Entity::Type entity) {
		return entities.find(entity)->second;
	}

	std::map<Entity::Type, MetaData::EntityData, MetaData::Service::EntitySorter>& MetaData::Service::getEntitiesData() {
		return entities;
	}
}