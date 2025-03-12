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

		//Get child
		auto* child = std::get_if<Child>(&relation);

		if (!child) {
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
		else {
			return {
				{"Name", name},
				{"Prefab_ID", prefab_id},
				{"Prefab_Override", prefab_override},
				{"B_Locked", b_locked},
				{"Layer_ID", layer_id},
				{"Layer_Order", layer_order},
				{"Tags", tags},
				{"Parent", child->parent }
			};
		}
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

		//Get Parent
		if (data.contains("Parent")) {
			Child temp_child;
			temp_child.parent = data["Parent"].get<std::string>();
			relation = temp_child;
		}
		else {
			relation = Parent();
		}

		//Update relation
		NIKE_METADATA_SERVICE->updateRelation();
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
			if (name.find(def_name) != std::string::npos) {
				for (auto const& data : entities) {
					if (data.second.name == name) return data.first;
				}

				return std::nullopt;
			}
			else {
				return std::nullopt;
			}
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

	bool MetaData::Service::checkEntityTagExist(Entity::Type entity) const
	{
		// Check if the entity exists in the entities map
		auto it = entities.find(entity);
		if (it != entities.end()) {
			// Iterate over the entity's tags and check if any exist in entity_tags
			for (const auto& tag : it->second.tags) {
				if (entity_tags.find(tag) != entity_tags.end()) {
					return true; 
				}
			}
		}

		return false;
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

	void MetaData::Service::updateRelation() {

		//Set of parents & childrens
		std::unordered_map<std::string, Parent*> parents;
		std::unordered_map<std::string, Child*> childs;

		//Iterate through entities
		for (auto& data : entities) {

			//Get parent
			auto* parent = std::get_if<Parent>(&data.second.relation);
			if (parent) {
				parent->childrens.clear();
				parents[data.second.name] = parent;
			}

			//Get child
			auto* child = std::get_if<Child>(&data.second.relation);
			if (child) {
				childs[data.second.name] = child;
			}
		}

		//Update each set properly
		for (auto& child : childs) {
			auto parent_it = parents.find(child.second->parent);
			if (parent_it != parents.end()) {
				parent_it->second->childrens.insert(child.first);
			}
			else {
				child.second->parent = "";
			}
		}
	}

	void MetaData::Service::setEntityParentRelation(Entity::Type entity) {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		//Get child
		auto* child = std::get_if<Child>(&entities.at(entity).relation);

		//Skip if relation is not a child
		if (!child) return;

		//Set relation
		entities.at(entity).relation = Parent();

		//Update relation
		updateRelation();
	}

	void MetaData::Service::setEntityChildRelation(Entity::Type entity) {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		//Get parent
		auto* parent = std::get_if<Parent>(&entities.at(entity).relation);

		//Skip if relation is not a parent
		if (!parent) return;

		//Set relation to child
		entities.at(entity).relation = Child();

		//Update relation
		updateRelation();
	}

	void MetaData::Service::setEntityChildRelationParent(Entity::Type entity, std::string const& parent_name) {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return;
		}

		//Get child
		auto* child = std::get_if<Child>(&entities.at(entity).relation);

		//Skip if relation is not child
		if (!child) return;

		//Check if parent is valid
		if (!checkParent(parent_name)) return;

		//Set child's parent
		child->parent = parent_name;

		//Update relation
		updateRelation();
	}

	std::variant<MetaData::Parent, MetaData::Child> MetaData::Service::getEntityRelation(Entity::Type entity) const {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return Parent();
		}

		//Return relation
		return entities.at(entity).relation;
	}

	std::vector<const char*> MetaData::Service::getAllParents() const {
		std::vector<const char*> parents;
		for (auto const& data : entities) {
			auto* parent = std::get_if<Parent>(&data.second.relation);

			if (parent) {
				parents.push_back(data.second.name.c_str());
			}
		}

		return parents;
	}

	bool MetaData::Service::checkParent(Entity::Type entity) const {
		//Check if entity exists
		if (entities.find(entity) == entities.end()) {
			NIKEE_CORE_WARN("Entity does not exist");
			return false;
		}

		//Get parent
		auto* parent = std::get_if<Parent>(&entities.at(entity).relation);

		//Check if relation is parent
		if (parent) {
			return true;
		}
		else {
			return false;
		}
	}

	bool MetaData::Service::checkParent(std::string const& parent_name) const {

		//Get entity
		auto entity = getEntityByName(parent_name);

		//Check if entity is valid
		if (entity.has_value()) {
			//Get parent
			auto* parent = std::get_if<Parent>(&entities.at(entity.value()).relation);

			//Check if relation is parent
			if (parent) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
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

	nlohmann::json MetaData::Service::serializePrefabData(MetaData::EntityData const& metadata) const {

		nlohmann::json data;

		//Serialize layer id
		data["Layer ID"] = metadata.layer_id;

		//Serialize tags
		data["Tags"] = metadata.tags;

		//Serialize parent
		auto* child = std::get_if<Child>(&metadata.relation);
		if (child) {
			data["Parent"] = child->parent;
		}

		return data;
	}

	nlohmann::json MetaData::Service::serializePrefabOverrideData(MetaData::EntityData const& entity_data, MetaData::EntityData const& prefab_data) const {
		nlohmann::json data;

		//Check if layer ID is the same as prefab
		if (entity_data.layer_id != prefab_data.layer_id) {
			data["Layer ID"] = entity_data.layer_id;
		}

		//Check if tags is the same as prefab
		if (entity_data.tags != prefab_data.tags) {
			data["Tags"] = entity_data.tags;
		}

		//Serialize parent
		auto* e_child = std::get_if<Child>(&entity_data.relation);
		auto* p_child = std::get_if<Child>(&prefab_data.relation);
		if (e_child) {
			if (!p_child || (p_child && (p_child->parent != e_child->parent))) {
				data["Parent"] = e_child->parent;
			}
		}
		else {
			if (p_child) {
				data["Not Child"] = true;
			}
		}

		return data;
	}

	bool MetaData::Service::deserializePrefabData(Entity::Type entity, nlohmann::json const& data) {
		bool success = true;

		//Deserialize layer ID
		if (data.contains("Layer ID")) {
			NIKE_METADATA_SERVICE->setEntityLayerID(entity, data.at("Layer ID"));
		}
		else {
			success = false;
		}

		//Get tags
		if (data.contains("Tags") && data["Tags"].is_array()) {
			auto tags = data["Tags"].get<std::set<std::string>>();

			for (auto tag : tags) {
				NIKE_METADATA_SERVICE->addEntityTag(entity, tag);
			}
		}
		else {
			success = false;
		}

		//Get Parent
		if (data.contains("Parent")) {
			NIKE_METADATA_SERVICE->setEntityChildRelation(entity);
			NIKE_METADATA_SERVICE->setEntityChildRelationParent(entity, data["Parent"].get<std::string>());
		}
		else {
			NIKE_METADATA_SERVICE->setEntityParentRelation(entity);
		}

		//Check for prefab overriders
		if (data.contains("Not Child")) {
			NIKE_METADATA_SERVICE->setEntityParentRelation(entity);
		}

		return success;
	}

	bool MetaData::Service::deserializePrefabData(MetaData::EntityData& metadata, nlohmann::json const& data) {
		bool success = true;

		//Deserialize layer ID
		if (data.contains("Layer ID")) {
			metadata.layer_id = data.at("Layer ID");
		}
		else {
			success = false;
		}

		//Get tags
		if (data.contains("Tags") && data["Tags"].is_array()) {
			auto tags = data["Tags"].get<std::set<std::string>>();

			for (auto tag : tags) {
				metadata.tags.insert(tag);
			}
		}
		else {
			success = false;
		}

		//Get Parent
		if (data.contains("Parent")) {
			Child temp_child;
			temp_child.parent = data["Parent"].get<std::string>();
			metadata.relation = temp_child;
		}
		else {
			metadata.relation = Parent();
		}

		//Check for prefab overriders
		if (data.contains("Not Child")) {
			metadata.relation = Parent();
		}

		return success;
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