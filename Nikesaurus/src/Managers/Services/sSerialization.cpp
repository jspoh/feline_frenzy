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

	nlohmann::json Serialization::Service::serializeLayer(Scenes::Layer const& layer) {
		//Json Data
		nlohmann::json data;

		////Layer data
		//nlohmann::json l_data;
		//l_data["name"] = layer.getLayerName();
		//l_data["id"] = layer.getLayerIndex();
		//data.push_back(l_data);

		////Iterate through all entities
		//for (auto const& entity : layer.getAllEntities()) {
		//	nlohmann::json e_data;
		//	e_data["Entity"] = serializeEntity(entity);
		//	e_data["Entity"]["id"] = entity;

		//	data.push_back(e_data);
		//}

		return data;
	}

	void Serialization::Service::deserializeEntity(Scenes::Layer& layer, nlohmann::json const& data) {

	}

	void Serialization::Service::saveEntityToFile(Entity::Type entity, std::string const& file_path) {
		//Json Data
		nlohmann::json data = serializeEntity(entity);

		//Open file stream
		std::fstream file(file_path, std::ios::out);

		//Store data
		file << data.dump(4);

		//Close file
		file.close();
	}

	void Serialization::Service::loadEntityFromFile(Entity::Type entity, std::string const& file_path) {
		//Json Data
		nlohmann::json data;

		//Open file stream
		std::fstream file(file_path, std::ios::in);

		//Read data from file
		file >> data;

		//Deserialize data
		deserializeEntity(entity, data);

		//Close file
		file.close();
	}

	void Serialization::Service::saveSceneToFile(std::string const& file_path) {
		//Json Data
		nlohmann::json data;

		//Layers in scene
		auto& layers = NIKE_SCENES_SERVICE->getCurrScene()->getLayers();

		//Iterate through all layers in current scene
		for (auto& layer : layers) {
			nlohmann::json l_data;
			l_data["Layer"] = serializeLayer(*layer);

			data.push_back(l_data);
		}

		//Open file stream
		std::fstream file(file_path, std::ios::out);

		//Store data
		file << data.dump(4);

		//Close file
		file.close();
	}

	void Serialization::Service::loadSceneFromFile(std::string const& file_path) {

	}
}

