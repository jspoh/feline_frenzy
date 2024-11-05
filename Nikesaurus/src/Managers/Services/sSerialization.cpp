/*****************************************************************//**
 * \file   mSerialization.cpp
 * \brief  Serialization manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/sSerialization.h"

namespace NIKE {
	/*****************************************************************//**
	* Component Serializer
	*********************************************************************/
	nlohmann::json Serialization::CompSerializer::serializeComponent(std::string const& comp_name, const void* comp) const {
		return serializers.at(comp_name)(comp);
	}

	void Serialization::CompSerializer::deserializeComponent(std::string const& comp_name, void* comp, nlohmann::json const& data) const {
		return deserializers.at(comp_name)(comp, data);
	}

	/*****************************************************************//**
	* Serialization Services
	*********************************************************************/
	nlohmann::json Serialization::Service::serializeEntity(Entity::Type entity) {

		//Create new data
		nlohmann::json data;

		//Iterate through all comp
		for (auto const& comp : NIKE_ECS_MANAGER->getAllComponents(entity)) {
			data["Components"][comp.first] = comp_registry->serializeComponent(comp.first, comp.second);
		}

		return data;
	}

	void Serialization::Service::deserializeEntity(Entity::Type entity, nlohmann::json const& data) {

		//Iterate through all components stored within data
		for (auto const& [comp_name, comp_data] : data["Components"].items()) {
			Component::Type comp_type = NIKE_ECS_MANAGER->getComponentType(comp_name);

			//Check if component is already present, if not add component
			if (!NIKE_ECS_MANAGER->checkEntityComponent(entity, comp_type)) {
				NIKE_ECS_MANAGER->addDefEntityComponent(entity, comp_type);
			}

			//Deserialize data into component
			comp_registry->deserializeComponent(comp_name, NIKE_ECS_MANAGER->getEntityComponent(entity, comp_type), comp_data);
		}
	}

	void Serialization::Service::saveEntityToFile(Entity::Type entity, std::string const& file_path) {
		//Json Data
		nlohmann::json data = serializeEntity(entity);

		//Save entity into file
		std::fstream file(file_path, std::ios::out);

		//Store data
		file << data.dump(4);

		//Close file
		file.close();
	}

	void Serialization::Service::loadEntityFromFile(Entity::Type entity, std::string const& file_path) {
		//Json Data
		nlohmann::json data;

		//Load entity from file
		std::fstream file(file_path, std::ios::in);

		//Read data from file
		file >> data;

		//Deserialize data
		deserializeEntity(entity, data);

		//Close file
		file.close();
	}
}

