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

		//If there are no components
		if (!data.contains("Components"))
			return;

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

		//Return if there is no data
		if (!std::filesystem::exists(file_path))
			return;

		//Read data from file
		file >> data;

		//Return if there is no data
		if (data.empty())
			return;

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

		//UI Entities
		std::unordered_map<Entity::Type, std::string> ui_entities;
		for (auto const& entity : NIKE_UI_SERVICE->getAllButtons()) {
			ui_entities.emplace(entity.second.first, entity.first);
		}

		//Iterate through all layers in current scene
		for (auto const& layer : layers) {

			//Serialize layer
			nlohmann::json l_data;
			l_data["Layer"] = layer->serialize();

			//Create json array
			l_data["Layer"]["Entities"] = nlohmann::json::array();

			//Iterate through all entities
			for (auto const& entity : NIKE_ECS_MANAGER->getAllEntities()) {
				//Skip entities not present in layer
				if (layer->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity))
					continue;

				//Entity data
				nlohmann::json e_data;

				//Serialize entity
				e_data["Entity"] = serializeEntity(entity);
				e_data["Entity"]["Layer ID"] = NIKE_ECS_MANAGER->getEntityLayerID(entity);

				//If entity is a UI Entity
				if (ui_entities.find(entity) != ui_entities.end()) {
					e_data["Entity"]["UI ID"] = ui_entities.at(entity);
				}

				//Push entity into layer data
				l_data["Layer"]["Entities"].push_back(e_data);
			}

			//Push layer data into main data
			data.push_back(l_data);
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

		//Return if there is no data
		if (!std::filesystem::exists(file_path))
			return;

		//Read data from file
		file >> data;

		//Return if there is no data
		if (data.empty())
			return;

		//Iterate through all layer data
		for (const auto& l_data : data) {

			//Deserialize layer
			if (!NIKE_SCENES_SERVICE->getCurrScene()->checkLayer(l_data.at("Layer").at("ID").get<int>())) {
				auto layer = NIKE_SCENES_SERVICE->getCurrScene()->createLayer();
				layer->deserialize(l_data.at("Layer"));
			}
			else {
				NIKE_SCENES_SERVICE->getCurrScene()->getLayer(l_data.at("Layer").at("ID").get<int>())->deserialize(l_data.at("Layer"));
			}

			//Iterate through all entities within layer
			for (const auto& e_data : l_data["Layer"]["Entities"]) {

				//Deserialize all entities
				Entity::Type entity = NIKE_ECS_MANAGER->createEntity();
				deserializeEntity(entity, e_data.at("Entity"));
				NIKE_ECS_MANAGER->setEntityLayerID(entity, e_data.at("Entity").at("Layer ID").get<unsigned int>());

				//Check if entity is a UI entity
				if (e_data.at("Entity").contains("UI ID")) {
					NIKE_UI_SERVICE->ui_entities.emplace(e_data.at("Entity").at("UI ID").get<std::string>(), std::make_pair(entity, false));
				}
			}
		}

		//Close file
		file.close();

		// Save file path
		curr_scene_file = file_path;
	}

	std::string const& Serialization::Service::getCurrSceneFile() const {
		return curr_scene_file;
	}
}

