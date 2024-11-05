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
		for (auto const& layer : layers) {
			nlohmann::json l_data;
			l_data["Layer"] = layer->serialize();

			data.push_back(l_data);
		}

		//Iterate through all entities
		for (auto const& entity : NIKE_ECS_MANAGER->getAllEntities()) {
			nlohmann::json e_data;
			e_data["Entity"] = serializeEntity(entity);
			e_data["Entity"]["ID"] = entity;
			e_data["Entity"]["Layer ID"] = NIKE_ECS_MANAGER->getEntityLayerID(entity);
			data.push_back(e_data);
		}

		//Open file stream
		std::fstream file(file_path, std::ios::out | std::ios::trunc);

		//Store data
		file << data.dump(4);

		//Close file
		file.close();
	}

	void Serialization::Service::loadSceneFromFile(std::string const& file_path) {
		//Json Data
		nlohmann::json data;

		//Open file stream
		std::fstream file(file_path, std::ios::in);

		//Read data from file
		file >> data;

		//Return if there is no data
		if (data.empty())
			return;

		//Iterate through all the data;
		for (auto& item : data) {
			if (item.contains("Layer")) {
				if (!NIKE_SCENES_SERVICE->getCurrScene()->checkLayer(item.at("Layer").at("ID").get<int>())) {
					auto layer = NIKE_SCENES_SERVICE->getCurrScene()->createLayer();
					layer->deserialize(item.at("Layer"));
				}
				else {
					NIKE_SCENES_SERVICE->getCurrScene()->getLayer(item.at("Layer").at("ID").get<int>())->deserialize(item.at("Layer"));
				}
			}

			if (item.contains("Entity")) {
				Entity::Type entity = NIKE_ECS_MANAGER->createEntity();
				deserializeEntity(entity, item.at("Entity"));
				NIKE_ECS_MANAGER->setEntityLayerID(entity, item.at("Entity").at("Layer ID").get<unsigned int>());
			}
		}

		//Close file
		file.close();
	}
}

