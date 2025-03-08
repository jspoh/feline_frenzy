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

		//Parent entity
		struct Parent {
			std::set<std::string> childrens;

			Parent() {};
		};

		//Children entity
		struct Child {
			std::string parent;

			Child() : parent{ "" } {}
		};

		//Data Type
		struct EntityData {

			//Name
			std::string name;

			//Prefab tag
			std::string prefab_id;

			//Override data for variant prefab
			nlohmann::json prefab_override;

			//Boolean for locking entities in editor
			bool b_locked;

			//Layer ID
			unsigned int layer_id;

			//Layer order
			size_t layer_order;

			//Dynamic tagging
			std::set<std::string> tags;

			//Parent or child
			std::variant<Parent, Child> relation;

			//Constructors
			EntityData() : name{ "entity_" }, prefab_id{ "" }, b_locked{ false }, layer_id{ 0 }, layer_order{ 0 }, relation{ Parent() } {}
			EntityData(std::string const& name)
				: name{ name }, prefab_id{ "" }, b_locked{ false }, layer_id{ 0 }, layer_order{ 0 }, relation{ Parent() } {}

			//Serialize data
			nlohmann::json serialize() const;

			//Deserialize data
			void deserialize(nlohmann::json const& data);
		};

		//Meta Data Management
		class Service : public Events::IEventListener<Coordinator::EntitiesChanged> {
		private:

			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			//Entity Types
			std::set<std::string> entity_tags;

			//Default entity name
			std::string def_name;

			//Ecs entities
			std::set<Entity::Type> ecs_entities;

			//Entities name mapping
			std::unordered_map<std::string, Entity::Type> entity_names;
				 
			//Sort entities
			struct EntitySorter {
				bool operator()(Entity::Type const& e1, Entity::Type const& e2) const {
					return e1 < e2;
				}
			};

			//Set of active entities
			std::map<Entity::Type, EntityData, EntitySorter> entities;

			//Entities to destroy
			std::set<Entity::Type> entities_to_destroy;

			//On entities changed event
			void onEvent(std::shared_ptr<Coordinator::EntitiesChanged> event) override;

			//Update entities data
			void updateData();

		public:

			//Default constructor
			Service() = default;

			//Init meta data manager
			void init(std::string const& def_entity_name = "entity_");

			//Update
			void update();

			//Check if type is valid
			bool isTagValid(std::string const& tag);

			//Add new entity type
			void registerTag(std::string const& tag);

			//Remove entity type
			void unregisterTag(std::string const& tag);

			//Get entity types
			std::set<std::string>const& getRegisteredTags() const;

			//Check Entity Name Taken
			bool isNameValid(std::string const& name) const;

			//Set Entity Name
			void setEntityName(Entity::Type entity, std::string const& name);

			//Get Entity name
			std::string getEntityName(Entity::Type entity) const;

			//Get Entity By Name
			std::optional<Entity::Type> getEntityByName(std::string const& name) const;

			//Set Entity Master prefab
			void setEntityPrefabID(Entity::Type entity, std::string const& prefab_id);

			//Get Entity Master prefab
			std::string getEntityPrefabID(Entity::Type entity) const;

			//Set Entity prefab override
			void setEntityPrefabOverride(Entity::Type entity, nlohmann::json const& data);

			//Get Entity prefab override
			nlohmann::json getEntityPrefabOverride(Entity::Type entity) const;

			//Add Entity Tag
			void addEntityTag(Entity::Type entity, std::string const& tag);

			//Remove Entity Tag
			void removeEntityTag(Entity::Type entity, std::string const& tag);

			//Get Entity Tags
			std::set<std::string> getEntityTags(Entity::Type entity);

			//Set Entity active State
			void destroyEntity(Entity::Type entity);

			//Set all entities active state
			void destroyAllEntities();

			//Set Entity locked or unlocked in editor
			void setEntityLocked(Entity::Type entity, bool b_locked);

			//Set all entities locked or unlocked in editor
			void setEntitiesLocked(bool b_locked);

			//Check if entity is locked
			bool checkEntityLocked(Entity::Type entity) const;

			//Set Entity layer ID
			void setEntityLayerID(Entity::Type entity, unsigned int layer_id);

			//Get Entity layer ID
			unsigned int getEntityLayerID(Entity::Type entity) const;

			//Set Entity layer order
			void setEntityLayerOrder(Entity::Type entity, size_t layer_id);

			//Get Entity layer order
			size_t getEntityLayerOrder(Entity::Type entity) const;

			//Update relation
			void updateRelation();

			//Set Entity Relation Parent Type
			void setEntityParentRelation(Entity::Type entity);

			//Set Entity Relation Child Type
			void setEntityChildRelation(Entity::Type entity);

			//Set Entity Child's Parent
			void setEntityChildRelationParent(Entity::Type entity, std::string const& parent_name);

			//Get Entity Relation
			std::variant<Parent, Child> getEntityRelation(Entity::Type entity) const;

			//Get all parents
			std::vector<const char*> getAllParents() const;

			//Check if parent is valid
			bool checkParent(Entity::Type entity) const;

			//Check if parent is valid
			bool checkParent(std::string const& parent_name) const;

			//Clone MetaData except for name
			void cloneEntityData(Entity::Type entity, Entity::Type clone);

			//Serialize entity
			nlohmann::json serializeEntityData(Entity::Type entity) const;

			//Deserialize entity
			void deserializeEntityData(Entity::Type entity, nlohmann::json const& data);

			//Serialize Prefab metadata
			nlohmann::json serializePrefabData(MetaData::EntityData const& metadata) const;

			//Serialize Prefab metadata
			nlohmann::json serializePrefabOverrideData(MetaData::EntityData const& entity_data, MetaData::EntityData const& prefab_data) const;

			//Deserialize Prefab metadata
			bool deserializePrefabData(Entity::Type entity, nlohmann::json const& data);

			//Deserialize Prefab metadata
			bool deserializePrefabData(MetaData::EntityData& metadata, nlohmann::json const& data);

			//Get first entity in list
			Entity::Type getFirstEntity() const;

			//Get entities by type
			std::set<Entity::Type> getEntitiesByTag(std::string const& tag) const;

			//Get entity metadata
			std::optional<EntityData> getEntityDataCopy(Entity::Type entity) const;

			//Get entities data
			std::map<Entity::Type, EntityData, EntitySorter> getEntitiesData() const;

			//Reset manager
			void reset();

			//Serialize data
			nlohmann::json serialize() const;

			//Deserialize data
			void deserialize(nlohmann::json const& data);
		};
	}
}

#endif //!M_ENTITY_HPP