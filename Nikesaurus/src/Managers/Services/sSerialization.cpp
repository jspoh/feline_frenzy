/*****************************************************************//**
 * \file   sSerialization.cpp
 * \brief  Serialization manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (80%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (20%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
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

	nlohmann::json Serialization::CompSerializer::serializeOverrideComponent(std::string const& comp_name, const void* comp, const void* other_comp) const {
		return override_serializers.at(comp_name)(comp, other_comp);
	}

	void Serialization::CompSerializer::deserializeOverrideComponent(std::string const& comp_name, void* comp, nlohmann::json const& data) const {
		return override_deserializers.at(comp_name)(comp, data);
	}

	/*****************************************************************//**
	* Serialization Services
	*********************************************************************/

	nlohmann::json Serialization::Service::serializeComponent(std::string const& comp_name, const void* comp) const {
		return comp_registry->serializeComponent(comp_name, comp);
	}

	void Serialization::Service::deserializeComponent(std::string const& comp_name, void* comp, nlohmann::json const& data) const {
		comp_registry->deserializeComponent(comp_name, comp, data);
	}

	nlohmann::json Serialization::Service::serializeOverrideComponent(std::string const& comp_name, const void* comp, const void* other_comp) const {
		return comp_registry->serializeOverrideComponent(comp_name, comp, other_comp);
	}

	void Serialization::Service::deserializeOverrideComponent(std::string const& comp_name, void* comp, nlohmann::json const& data) const {
		comp_registry->deserializeOverrideComponent(comp_name, comp, data);
	}

	std::unordered_map<std::string, std::function<std::shared_ptr<void>()>>const& Serialization::Service::getCompFuncs() const {
		return comp_funcs;
	}

	/*****************************************************************//**
	* Grid
	*********************************************************************/

	void Serialization::Service::saveGridToFile(const std::string& file_path)
	{
		nlohmann::json grid_data = NIKE_MAP_SERVICE->serialize();

		// Get file path to seri
		std::fstream file(file_path, std::ios::out);

		if (!std::filesystem::exists(file_path))
		{
			NIKEE_CORE_ERROR("File does not exist!");
			return;
		}

		// Save data into file
		file << grid_data.dump(4);

		file.close();
	}

	void Serialization::Service::loadGridFromFile(const std::string& file_path)
	{
		// Get file path to seri
		std::fstream file(file_path, std::ios::in);

		if (!file.is_open()) {
			NIKEE_CORE_ERROR("Failed to open .map file for loading: " + file_path);
			return;
		}

		nlohmann::json grid_data;
		file >> grid_data;
		file.close();

		if (grid_data.empty()) return;

		NIKE_MAP_SERVICE->deserialize(grid_data);
	}

	/*****************************************************************//**
	* Entity
	*********************************************************************/
	nlohmann::json Serialization::Service::serializeEntity(Entity::Type entity) {

		//Create new data
		nlohmann::json data;

		//Iterate through all comp
		for (auto const& comp : NIKE_ECS_MANAGER->getAllEntityComponents(entity)) {
			data["Components"][comp.first] = comp_registry->serializeComponent(comp.first, comp.second.get());
		}

		return data;
	}

	bool Serialization::Service::deserializeEntity(Entity::Type entity, nlohmann::json const& data) {

		//Boolean for flagging errors in deserializing
		bool success = true;

		//If there are no components
		if (!data.contains("Components"))
			return success;

		//Iterate through all components stored within data
		for (auto const& [comp_name, comp_data] : data["Components"].items()) {

			//Check if component exists within the system
			if (NIKE_ECS_MANAGER->checkComponentType(comp_name)) {
				Component::Type comp_type = NIKE_ECS_MANAGER->getComponentType(comp_name);

				//Check if component is already present, if not add component
				if (!NIKE_ECS_MANAGER->checkEntityComponent(entity, comp_type)) {
					NIKE_ECS_MANAGER->addDefEntityComponent(entity, comp_type);
				}

				//Deserialize data into component
				comp_registry->deserializeComponent(comp_name, NIKE_ECS_MANAGER->getEntityComponent(entity, comp_type).get(), comp_data);
			}
			else {
				success = false;
			}
		}

		return success;
	}

	void Serialization::Service::savePrefab(std::unordered_map<std::string, std::shared_ptr<void>> const& comps, std::string const& file_path, MetaData::EntityData const& meta_data) {

		//Create new data
		nlohmann::json data;

		//Iterate through all comp
		for (auto const& comp : comps) {
			data["Components"][comp.first] = comp_registry->serializeComponent(comp.first, comp.second.get());
		}

		//Serialize prefab metadata
		data["MetaData"] = NIKE_METADATA_SERVICE->serializePrefabData(meta_data);

		//Open file stream
		std::fstream file(file_path, std::ios::out);

		//Store data
		file << data.dump(4);

		//Close file
		file.close();
	}

	void Serialization::Service::loadPrefab(std::unordered_map<std::string, std::shared_ptr<void>>& comps, MetaData::EntityData& meta_data, std::string const& file_path) {

		//Boolean for flagging errors in deserializing
		bool success = true;

		//Json Data
		nlohmann::json data;

		//Open file stream
		std::fstream file(file_path, std::ios::in | std::ios::out);

		//Return if there is no data
		if (!std::filesystem::exists(file_path))
			return;

		//Read data from file
		file >> data;

		//Return if there is no data
		if (data.empty())
			return;

		//If there are no components
		if (data.contains("Components")) {
			//Iterate through all components stored within data
			for (auto const& [comp_name, comp_data] : data["Components"].items()) {
				//Check for comp functions validity
				if (comp_funcs.find(comp_name) != comp_funcs.end()) {
					comps[comp_name] = comp_funcs[comp_name]();

					//Deserialize data into component
					comp_registry->deserializeComponent(comp_name, comps[comp_name].get(), comp_data);
				}
				else {
					success = false;
				}
			}
		}
		else {
			success = false;
		}

		//Load prefab metadata
		if (data.contains("MetaData")) {
			success = NIKE_METADATA_SERVICE->deserializePrefabData(meta_data, data.at("MetaData"));
		}
		else {
			success = false;
		}

		//Error encountered in deserializing
		if (!success) {

			//Save prefab again
			savePrefab(comps, file_path, meta_data);
		}

		//Close file
		file.close();
	}

	nlohmann::json Serialization::Service::serializePrefabOverrides(Entity::Type entity, std::string const& prefab_id) {

		//Create new data
		nlohmann::json data;

		//Map to array of component type
		std::unordered_map<std::string, std::shared_ptr<void>> prefab_comps;

		//Layer ID
		MetaData::EntityData metadata;

		//Load prefab into prefab comps
		loadPrefab(prefab_comps, metadata, NIKE_ASSETS_SERVICE->getAssetPath(prefab_id).string());

		//Iterate through all comp
		for (auto const& comp : NIKE_ECS_MANAGER->getAllEntityComponents(entity)) {
			if (prefab_comps.find(comp.first) != prefab_comps.end()) {
				data["Components"][comp.first] = comp_registry->serializeOverrideComponent(comp.first, comp.second.get(), prefab_comps.at(comp.first).get());
			}
			else {
				data["Components"][comp.first] = comp_registry->serializeComponent(comp.first, comp.second.get());
			}
		}

		//Serialize prefab override data
		data["MetaData"] = NIKE_METADATA_SERVICE->serializePrefabOverrideData(NIKE_METADATA_SERVICE->getEntityDataCopy(entity).has_value() ? NIKE_METADATA_SERVICE->getEntityDataCopy(entity).value() : MetaData::EntityData(), metadata);

		return data;
	}

	bool Serialization::Service::deserializePrefabOverrides(Entity::Type entity, nlohmann::json const& data) {
		//Boolean for flagging errors in deserializing
		bool success = true;

		//If there are no components
		if (!data.contains("Components"))
			return success;

		//Iterate through all components stored within data
		for (auto const& [comp_name, comp_data] : data["Components"].items()) {
			//Check if component exists within the system
			if (NIKE_ECS_MANAGER->checkComponentType(comp_name)) {
				Component::Type comp_type = NIKE_ECS_MANAGER->getComponentType(comp_name);

				//Check if component is already present, if not add component
				if (!NIKE_ECS_MANAGER->checkEntityComponent(entity, comp_type)) {
					NIKE_ECS_MANAGER->addDefEntityComponent(entity, comp_type);
				}

				//Deserialize data into component
				comp_registry->deserializeOverrideComponent(comp_name, NIKE_ECS_MANAGER->getEntityComponent(entity, comp_type).get(), comp_data);
			}
			else {
				success = false;
			}
		}

		//Deserialize prefab data
		if (data.contains("MetaData")) {
			NIKE_METADATA_SERVICE->deserializePrefabData(entity, data.at("MetaData"));
		}

		return success;
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
		//Prefab check
		if (std::filesystem::path(file_path).extension().string() == "prefab") {
			NIKEE_CORE_WARN("Use load entity from prefab instead!");
			return;
		}

		//Json Data
		nlohmann::json data;

		//Open file stream
		std::fstream file(file_path, std::ios::in | std::ios::out);

		//Return if there is no data
		if (!std::filesystem::exists(file_path))
			return;

		//Read data from file
		file >> data;

		//Return if there is no data
		if (data.empty())
			return;

		//Deserialize data
		if (!deserializeEntity(entity, data)) {

			//Error encountered in deserialization
			saveEntityToFile(entity, file_path);
		}

		//Close file
		file.close();
	}

	void Serialization::Service::loadEntityFromPrefab(Entity::Type entity, std::string const& prefab_id) {

		//Prefab check
		if (prefab_id.find(".prefab") == std::string::npos) {
			NIKEE_CORE_WARN("Not a valid prefab id!");
			return;
		}

		//Json Data
		nlohmann::json data;

		//Get file path
		auto file_path = NIKE_ASSETS_SERVICE->getAssetPath(prefab_id);

		//Open file stream
		std::fstream file(file_path, std::ios::in | std::ios::out);

		//Return if there is no data
		if (!std::filesystem::exists(file_path))
			return;

		//Read data from file
		file >> data;

		//Return if there is no data
		if (data.empty())
			return;

		//Deserialize data
		if (!deserializeEntity(entity, data) || !data.contains("MetaData")) {

			//Error encountered in deserialization
			savePrefab(NIKE_ECS_MANAGER->getAllEntityComponents(entity), file_path.string(), NIKE_METADATA_SERVICE->getEntityDataCopy(entity).has_value() ? NIKE_METADATA_SERVICE->getEntityDataCopy(entity).value() : MetaData::EntityData());
		}

		//Deserialize prefab data
		if (data.contains("MetaData")) {
			NIKE_METADATA_SERVICE->deserializePrefabData(entity, data.at("MetaData"));
		}

		//Close file
		file.close();
	}

	nlohmann::json Serialization::Service::serializePlayerData(Entity::Type player) {
		// Create a new JSON object
		nlohmann::json playerData;

		// Get player health
		if (auto healthComp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(player)) {
			playerData["Health"] = healthComp.value().get().health;
		}

		// Get equipped element
		if (auto elementComp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(player)) {
			playerData["Element"] = static_cast<int>(elementComp.value().get().element);
		}

		return playerData;
	}


	bool Serialization::Service::deserializePlayerData(Entity::Type player, nlohmann::json const& data) {
		// Check if the JSON contains player data
		if (data.contains("Health")) {
			if (auto healthComp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(player)) {
				healthComp.value().get().health = data["Health"];
			}
		}

		if (data.contains("Element")) {
			if (auto elementComp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(player)) {
				elementComp.value().get().element = static_cast<Element::Elements>(data["Element"]);
			}
		}

		return true; // Return success
	}


	/*****************************************************************//**
	* Scenes
	*********************************************************************/

	std::string const& Serialization::Service::getCurrSceneFile() const {
		return curr_scene_file;
	}

	void Serialization::Service::saveSceneToFile(std::string const& file_path) {

		//Json Data
		nlohmann::json data;

		//Audio Data
		nlohmann::json channels_data;
		channels_data["Channels"] = NIKE_AUDIO_SERVICE->serializeAudioChannels();
		data.push_back(channels_data);
		
		// Grid Data
		// Extract grid_id from the scene file name
		std::string grid_id = Utility::extractFileName(file_path) + ".grid";

		// Check if the "Grids" folder contains the .grid file
		//if (std::filesystem::exists(grid_path)) {
		// Add grid ID data only if the file exists
		nlohmann::json m_data;
		m_data["Grid ID"] = grid_id;
		data.push_back(m_data);

		// Camera Component Data
		nlohmann::json cam_data;
		cam_data["Camera"] = NIKE_CAMERA_SERVICE->serializeCamera();
		data.push_back(cam_data);

		// Camera Component Data
		nlohmann::json mt_data;
		mt_data["MetaData"] = NIKE_METADATA_SERVICE->serialize();
		data.push_back(mt_data);

		//UI Entities
		auto const& ui_entities = NIKE_UI_SERVICE->getAllButtons();
		std::unordered_map<Entity::Type, std::string> ui_entity_to_ref;
		for (auto it = ui_entities.begin(); it != ui_entities.end(); ++it) {
			ui_entity_to_ref.emplace(it->second.entity_id, it->first);
		}

		//Layers in scene
		auto& layers = NIKE_SCENES_SERVICE->getLayers();

		//Scene data
		nlohmann::json scn_data;
		scn_data["Layer Count"] = layers.size();
		data.push_back(scn_data);

		//Iterate through all layers in current scene
		for (auto const& layer : layers) {

			//Serialize layer
			nlohmann::json l_data;

			//Serialize layer
			l_data["Layer"] = layer->serialize();

			//Create json array
			l_data["Layer"]["Entities"] = nlohmann::json::array();

			//Iterate through all entities in layer
			for (auto& entity : layer->getEntitites()) {

				// skip 'built in' fps display entity
				const auto entity_components = NIKE_ECS_MANAGER->getAllEntityComponents(entity);
				const auto it = entity_components.find("Render::BuiltIn");
				if (it != entity_components.end()) {
					// is built in component, do not savee to scene
					continue;
				}

				//Entity data
				nlohmann::json e_data;

				//Check if prefab is valid
				auto prefab_id = NIKE_METADATA_SERVICE->getEntityPrefabID(entity);
				if (!prefab_id.empty()) {
					//Serialize override data
					NIKE_METADATA_SERVICE->setEntityPrefabOverride(entity, NIKE_SERIALIZE_SERVICE->serializePrefabOverrides(entity, prefab_id));
				}
				else {
					//Serialize entity data
					e_data["Entity"] = serializeEntity(entity);
				}

				//Serialize metadata
				e_data["Entity"]["MetaData"] = NIKE_METADATA_SERVICE->serializeEntityData(entity);

				//If entity is a UI Entity
				if (ui_entity_to_ref.find(entity) != ui_entity_to_ref.end()) {
					e_data["Entity"]["UI ID"] = ui_entity_to_ref.at(entity);
					e_data["Entity"]["UI Btn"] = ui_entities.at(ui_entity_to_ref.at(entity)).serialize();
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

		//Boolean for flagging out errors in deserializations
		bool success = true;

		//Return if there is no data
		if (!std::filesystem::exists(file_path))
			return;

		//Json Data
		nlohmann::json data;

		//Open file stream
		std::fstream file(file_path, std::ios::in | std::ios::out);

		//Read data from file
		file >> data;

		//Return if there is no data
		if (data.empty())
			return;


		//Iterate through all data
		for (const auto& l_data : data) {

			//Deserialize servies
			if (l_data.contains("Channels")) {
				NIKE_AUDIO_SERVICE->deserializeAudioChannels(l_data["Channels"]);
			}
			if (l_data.contains("Camera")) {
				NIKE_CAMERA_SERVICE->deserializeCamera(l_data["Camera"]);
			}
			if (l_data.contains("MetaData")) {
				NIKE_METADATA_SERVICE->deserialize(l_data["MetaData"]);
			}

			//Load map grid if a map file path is specified
			if (l_data.contains("Grid ID")) {
				std::string grid_id = l_data.value("Grid ID", "");
				std::string full_grid_path = NIKE_ASSETS_SERVICE->getAssetPath(grid_id).string();

				if (std::filesystem::exists(full_grid_path)) {
					// Deserialize map grid
					NIKE_SERIALIZE_SERVICE->loadGridFromFile(full_grid_path);
				}
				else {
					// Log error or handle missing map file
					NIKEE_CORE_ERROR("Map file not found: " + grid_id);
				}
			}

			//Check for layer count
			if (l_data.contains("Layer Count")) {
				for (int i = 1; i < l_data.value("Layer Count", 1); ++i) {
					NIKE_SCENES_SERVICE->createLayer();
				}
			}

			//If data contains layer
			if (l_data.contains("Layer") && l_data.at("Layer").contains("ID") && l_data.at("Layer").contains("Entities")) {

				//Deserialize layer
				if (!NIKE_SCENES_SERVICE->checkLayer(l_data.at("Layer").at("ID").get<int>())) {
					auto layer = NIKE_SCENES_SERVICE->createLayer();
					layer->deserialize(l_data.at("Layer"));
				}
				else {
					NIKE_SCENES_SERVICE->getLayer(l_data.at("Layer").at("ID").get<int>())->deserialize(l_data.at("Layer"));
				}

				//Iterate through all entities within layer
				for (const auto& e_data : l_data["Layer"]["Entities"]) {

					//Check if entity is a UI entity
					if (e_data.contains("Entity")) {

						//Deserialize all entities
						Entity::Type entity = NIKE_ECS_MANAGER->createEntity();

						//Deserialize entity metadata
						if (e_data.at("Entity").contains("MetaData")) {

							//Deserialize
							NIKE_METADATA_SERVICE->deserializeEntityData(entity, e_data.at("Entity").at("MetaData"));
							NIKE_METADATA_SERVICE->setEntityLayerID(entity, NIKE_METADATA_SERVICE->getEntityLayerID(entity));
							NIKE_METADATA_SERVICE->setEntityLayerOrder(entity, NIKE_METADATA_SERVICE->getEntityLayerOrder(entity));

							//Get entity prefab ID
							auto entity_prefab_id = NIKE_METADATA_SERVICE->getEntityPrefabID(entity);

							//Check if prefab id is present
							if (!entity_prefab_id.empty()) {

								//Load prefab
								loadEntityFromPrefab(entity, entity_prefab_id);

								//Apply overrides
								if (!deserializePrefabOverrides(entity, NIKE_METADATA_SERVICE->getEntityPrefabOverride(entity))) {
									success = false;
								}
							}
							else {
								if (!deserializeEntity(entity, e_data.at("Entity"))) {
									success = false;
								}
							}
						}

						//Check if entity is a UI entity
						if (e_data.at("Entity").contains("UI ID")) {

							UI::UIBtn btn;
							btn.deserialize(e_data.at("Entity").at("UI Btn"));
							btn.entity_id = entity;
							btn.b_hovered = false;
							NIKE_UI_SERVICE->getAllButtons()[e_data.at("Entity").at("UI ID").get<std::string>()] = btn;
						}
					}
				}
			}
		}

		// Save file path
		curr_scene_file = file_path;

		//Error in deserialization process
		if (!success) {
			saveSceneToFile(file_path);
		}

		//Close file
		file.close();
	}

	nlohmann::json Serialization::Service::loadJsonFile(std::string const& file_path) {
		//Json Data
		nlohmann::json data;

		//Open file stream
		std::fstream file(file_path, std::ios::in);

		//Return empty data if there is no data
		if (!std::filesystem::exists(file_path))
			return data;

		//Read data from file
		file >> data;

		//Return loaded json data
		return data;
	}
}
