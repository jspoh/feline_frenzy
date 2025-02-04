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

	nlohmann::json MetaData::EntityData::serialize() const {
		return {
			{"Name", name},
			{"Prefab_ID", prefab_id},
			{"Prefab_Override", prefab_override},
			{"B_Locked", b_locked},
			{"Layer_ID", layer_id},
			{"Layer_Order", layer_order},
			{"Tags", tags}
		};
	}

	void MetaData::EntityData::deserialize(nlohmann::json const& data) {

		//If there is a name present
		if (data.contains("Name")) {
			name = data["Name"].get<std::string>();
		}

		//Get default values
		prefab_id = data.value("Prefab_ID", "");
		prefab_override = data.value("Prefab_Override", nlohmann::json());
		b_locked = data.value("B_Locked", false);
		layer_id = data.value("Layer_ID", static_cast<unsigned int>(0));
		layer_order = data.value("Layer_Order", static_cast<unsigned int>(0));

		//Get tags
		if (data.contains("Tags") && data["Tags"].is_array()) {
			tags = data["Tags"].get<std::set<std::string>>();
		}
	}

	void MetaData::Service::onEvent(std::shared_ptr<Coordinator::EntitiesChanged> event) {

		//Get entities
		ecs_entities = event->entities;

		//Remove entities that are no longer in the ECS
		for (auto it = entities.begin(); it != entities.end();) {
			if (ecs_entities.find(it->first) == ecs_entities.end()) {
				//Remove entity from layer
				NIKE_SCENES_SERVICE->getLayer(it->second.layer_id)->removeEntity(it->first);

				//Erase entity
				it = entities.erase(it);
			}
			else {
				++it;
			}
		}

		//Update entities data
		updateData();
	}

	void MetaData::Service::updateData() {

		//Clear entity names & repopulate them with updated names
		entity_names.clear();

		//Add new entities from the ECS that are not yet in the editor
		int index = 0;
		for (auto& entity : ecs_entities) {

			//Update entities ref
			if (entities.find(entity) == entities.end()) {

				//Create identifier for entity
				char entity_name[32];
				snprintf(entity_name, sizeof(entity_name), (def_name + "%04d").data(), index);
				entities[entity].name = entity_name;

				//Set a proper layer ID
				setEntityLayerID(entity, 0);
			}
			else if (entities.at(entity).name.find(def_name) != std::string::npos) {

				//Create identifier for entity
				char entity_name[32];
				snprintf(entity_name, sizeof(entity_name), (def_name + "%04d").data(), index);
				entities[entity].name = entity_name;
			}

			//Populate entity name
			entity_names[entities[entity].name] = entity;

			//Increment index
			++index;
		}
	}

	void MetaData::Service::init(std::string const& def_entity_name) {

		def_name = def_entity_name;

		//Setup events listening
		std::shared_ptr<MetaData::Service> metadata_service(this, [](MetaData::Service*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Coordinator::EntitiesChanged>(metadata_service);
	}

	void MetaData::Service::update() {

		//Empty destroy entities queue
		if (!entities_to_destroy.empty()) {
			for (auto entity : entities_to_destroy) {
				if (NIKE_ECS_MANAGER->checkEntity(entity)) {
					NIKE_ECS_MANAGER->destroyEntity(entity);
				}
			}

			//Clear entities to destroy
			entities_to_destroy.clear();
		}
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

	std::set<std::string> const& MetaData::Service::getRegisteredTags() const {
		return entity_tags;
	}

	bool MetaData::Service::isNameValid(std::string const& name) const {
		return (entity_names.find(name) == entity_names.end() && name.find(def_name) == std::string::npos);
	}

	void MetaData::Service::setEntityName(Entity::Type entity, std::string const& name) {

		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		//Check if name has been taken
		if (!isNameValid(name)) {
			return;
		}

		//Set name
		entities.at(entity).name = name;

		//Populate entity name
		entity_names[entities[entity].name] = entity;
	}

	std::string MetaData::Service::getEntityName(Entity::Type entity) const {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return "";
		}

		return entities.at(entity).name;
	}

	std::optional<Entity::Type> MetaData::Service::getEntityByName(std::string const& name) const {
		if (entity_names.find(name) == entity_names.end()) {
			return  std::nullopt;
		}

		return entity_names.at(name);
	}

	void MetaData::Service::setEntityPrefabID(Entity::Type entity, std::string const& prefab_id) {

		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		//Check if id is valid
		if (!prefab_id.empty() && prefab_id.substr(prefab_id.find_first_of('.')) != ".prefab") {
			NIKEE_CORE_WARN("Trying To Set An Invalid Prefab ID!!!");
			return;
		}

		//Set prefab master id
		entities.at(entity).prefab_id = prefab_id;

		//Check if prefab id is valid
		if (!entities.at(entity).prefab_id.empty() && entities.at(entity).prefab_id.find(".prefab") != std::string::npos) {
			//Load entity with prefab
			NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(entity, prefab_id);
		}
	}

	std::string MetaData::Service::getEntityPrefabID(Entity::Type entity) const {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return "";
		}

		return entities.at(entity).prefab_id;
	}

	void MetaData::Service::setEntityPrefabOverride(Entity::Type entity, nlohmann::json const& data) {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		//Set prefab master id
		entities.at(entity).prefab_override = data;
	}

	nlohmann::json MetaData::Service::getEntityPrefabOverride(Entity::Type entity) const {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return "";
		}

		return entities.at(entity).prefab_override;
	}

	void MetaData::Service::addEntityTag(Entity::Type entity, std::string const& tag) {

		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		//Check if tag has been registered
		if (!isTagValid(tag)) {
			NIKEE_CORE_WARN("Tag not registered yet.");
			return;
		}

		//Set tag
		entities.at(entity).tags.insert(tag);
	}

	void MetaData::Service::removeEntityTag(Entity::Type entity, std::string const& tag) {

		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		//Check if tag has been registered
		if (!isTagValid(tag)) {
			NIKEE_CORE_WARN("Tag not registered yet.");
			return;
		}

		//Set tag
		entities.at(entity).tags.erase(tag);
	}

	void MetaData::Service::destroyEntity(Entity::Type entity) {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		//Check if entity is active
		entities_to_destroy.insert(entity);
	}

	void MetaData::Service::destroyAllEntities() {
		for (auto& e_data : entities) {
			entities_to_destroy.insert(e_data.first);
		}
	}

	void MetaData::Service::setEntityLocked(Entity::Type entity, bool b_locked) {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		entities.at(entity).b_locked = b_locked;
	}

	void MetaData::Service::setEntitiesLocked(bool b_locked) {
		for (auto& e_data : entities) {
			e_data.second.b_locked = b_locked;
		}
	}

	bool MetaData::Service::checkEntityLocked(Entity::Type entity) const {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			return false;
		}

		return entities.at(entity).b_locked;
	}

	void MetaData::Service::setEntityLayerID(Entity::Type entity, unsigned int layer_id) {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		//Check if layer ID is valid
		if (layer_id < 0 || layer_id >= NIKE_SCENES_SERVICE->getLayerCount()) {
			NIKEE_CORE_WARN("Layer ID out of range");
			return;
		}

		//Remove entity from every layer
		for (auto layer : NIKE_SCENES_SERVICE->getLayers()) {
			layer->removeEntity(entity);
		}

		//Set layer ID
		entities.at(entity).layer_id = layer_id;

		//Check if layer exists
		if (NIKE_SCENES_SERVICE->checkLayer(entities.at(entity).layer_id)) {

			//Insert entity into new layer
			auto layer = NIKE_SCENES_SERVICE->getLayer(entities.at(entity).layer_id);
			layer->insertEntity(entity);

			//Get layer order assigned
			entities.at(entity).layer_order = layer->getEntityOrder(entity);
		}
	}

	unsigned int MetaData::Service::getEntityLayerID(Entity::Type entity) const {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return 0;
		}

		//Return layer ID
		return entities.at(entity).layer_id;
	}

	void MetaData::Service::setEntityLayerOrder(Entity::Type entity, size_t layer_order) {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		//Check if layer ID is valid
		if (layer_order < 0 && layer_order >= NIKE_SCENES_SERVICE->getLayer(entities.at(entity).layer_id)->getEntitites().size()) {
			NIKEE_CORE_WARN("Layer Order out of range");
			return;
		}

		//Set layer Order
		entities.at(entity).layer_order = layer_order;

		//Set layer order within layer
		auto layer = NIKE_SCENES_SERVICE->getLayer(entities.at(entity).layer_id);
		layer->setEntityOrder(entity, entities.at(entity).layer_order);
	}

	size_t MetaData::Service::getEntityLayerOrder(Entity::Type entity) const {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return 0;
		}

		//Return layer ID
		return entities.at(entity).layer_order;
	}

	std::set<std::string> MetaData::Service::getEntityTags(Entity::Type entity) {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return std::set<std::string>();
		}

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
		entities.at(entity).b_locked = it_clone->second.b_locked;
		entities.at(entity).prefab_id = it_clone->second.prefab_id;
		entities.at(entity).prefab_override = it_clone->second.prefab_override;
		entities.at(entity).layer_id = it_clone->second.layer_id;
	}

	nlohmann::json MetaData::Service::serializeEntityData(Entity::Type entity) const {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			return nlohmann::json();
		}

		return entities.at(entity).serialize();
	}

	void MetaData::Service::deserializeEntityData(Entity::Type entity, nlohmann::json const& data) {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			return;
		}

		entities.at(entity).deserialize(data);
	}

	Entity::Type MetaData::Service::getFirstEntity() const {
		return entities.begin()->first;
	}

	std::optional<MetaData::EntityData> MetaData::Service::getEntityDataCopy(Entity::Type entity) const {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return std::nullopt;
		}

		return entities.at(entity);
	}

	std::map<Entity::Type, MetaData::EntityData, MetaData::Service::EntitySorter> MetaData::Service::getEntitiesData() const {
		return entities;
	}

	void MetaData::Service::reset() {
		entity_tags.clear();
	}

	nlohmann::json MetaData::Service::serialize() const {
		return {
		{"Entity_Tags", entity_tags}
		};
	}

	void MetaData::Service::deserialize(nlohmann::json const& data) {
		if (data.contains("Entity_Tags")) {
			for (const auto& tag : data.at("Entity_Tags").items()) {
				entity_tags.insert(tag.value());
			}
		}
	}
}