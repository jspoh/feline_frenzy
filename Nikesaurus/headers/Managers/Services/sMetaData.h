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

			//Prefab tag
			std::string prefab_id;

			//Override data for variant prefab
			nlohmann::json prefab_override;

			//Boolean for locking entities in editor
			bool b_locked;

			//Boolean for checking if entity is active
			bool b_isactive;

			//Dynamic tagging
			std::set<std::string> tags;

			//Constructors
			EntityData() : name{ "entity_" }, prefab_id{ "" }, b_isactive{ true }, b_locked{ false } {}
			EntityData(std::string const& name)
				: name{ name }, prefab_id{ "" }, b_isactive{ true }, b_locked{ false } {}

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

			//On entities changed event
			void onEvent(std::shared_ptr<Coordinator::EntitiesChanged> event) override;

			//Update entities data
			void updateData();

		public:

			//Default constructor
			Service() = default;

			//Init meta data manager
			void init(std::string const& def_entity_name = "entity_");

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
			void setEntityActive(Entity::Type entity, bool b_active);

			//Set all entities active state
			void setEntitiesActive(bool b_active);

			//Check if entity is active
			bool checkEntityActive(Entity::Type entity) const;

			//Set Entity locked or unlocked in editor
			void setEntityLocked(Entity::Type entity, bool b_locked);

			//Set all entities locked or unlocked in editor
			void setEntitiesLocked(bool b_locked);

			//Check if entity is locked
			bool checkEntityLocked(Entity::Type entity) const;

			//Clone MetaData except for name
			void cloneEntityData(Entity::Type entity, Entity::Type clone);

			//Get first entity in list
			Entity::Type getFirstEntity() const;

			//Get entities by type
			std::set<Entity::Type> getEntitiesByTag(std::string const& tag) const;

			//Get entity metadata
			std::optional<EntityData> getEntityData(Entity::Type entity) const;

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