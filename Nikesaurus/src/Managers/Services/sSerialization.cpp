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
		for (auto const& comp : NIKE_ECS_MANAGER->getAllEntityComponents(entity)) {
			data["Components"][comp.first] = comp_registry->serializeComponent(comp.first, comp.second.get());
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
			comp_registry->deserializeComponent(comp_name, NIKE_ECS_MANAGER->getEntityComponent(entity, comp_type).get(), comp_data);
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

		// Add the map file path at the top of the scene JSON
		std::string grid_id = Utility::extractFileName(file_path) + ".grid";
		nlohmann::json m_data;
		m_data["Grid ID"] = grid_id;
		data.push_back(m_data);

		//Layers in scene
		auto& layers = NIKE_SCENES_SERVICE->getLayers();

		//UI Entities
		std::unordered_map<Entity::Type, std::string> ui_entities;
		for (auto const& entity : NIKE_UI_SERVICE->getAllButtons()) {
			ui_entities.emplace(entity.second.entity_id, entity.first);
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
				//e_data["Entity"]["Entity Name"] = NIKE_LVLEDITOR_SERVICE->getEntityByType(entity);
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

			//Load map grid if a map file path is specified
			if (l_data.contains("Grid ID")) {
				std::string grid_id = l_data.at("Grid ID").get<std::string>();
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

			//If data contains layer
			if (l_data.contains("Layer")) {
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

					//Deserialize all entities
					Entity::Type entity = NIKE_ECS_MANAGER->createEntity();
					deserializeEntity(entity, e_data.at("Entity"));
					//NIKE_IMGUI_SERVICE->addEntityRef(e_data.at("Entity").at("Entity Name").get<std::string>(), entity);
					NIKE_ECS_MANAGER->setEntityLayerID(entity, e_data.at("Entity").at("Layer ID").get<unsigned int>());

					//Check if entity is a UI entity
					if (e_data.at("Entity").contains("UI ID")) {
						UI::UIBtn btn;
						btn.entity_id = NIKE_ECS_MANAGER->createEntity(NIKE_SCENES_SERVICE->getLayerCount() - 1);
						btn.b_hovered = false;
						NIKE_UI_SERVICE->ui_entities.emplace(e_data.at("Entity").at("UI ID").get<std::string>(), btn);
					}
				}
			}
		}

		//Close file
		file.close();

		// Save file path
		curr_scene_file = file_path;
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

	std::string const& Serialization::Service::getCurrSceneFile() const {
		return curr_scene_file;
	}

	void Serialization::Service::saveGridToFile(const std::string& file_path)
	{
		nlohmann::json grid_data = NIKE_MAP_SERVICE->serialize();

		// Get file path to seri
		std::fstream file(file_path, std::ios::out);

		if (!std::filesystem::exists(file_path))
			NIKEE_CORE_ERROR("File does not exist!");
		
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
		}

		nlohmann::json grid_data;
		file >> grid_data;
		file.close();

		NIKE_MAP_SERVICE->deserialize(grid_data);
	}

	//void Serialization::Service::loadMapFromFile(const std::string& file, std::shared_ptr<NIKE::Scenes::Layer>& background_layer, std::shared_ptr<NIKE::Scenes::Layer>& player_layer, std::vector<std::vector<int>>& grid, const NIKE::Math::Vector2<float>& center) {
	//	// Open Scene file
	//	std::ifstream ifs{ file, std::ios::in };
	//	if (!ifs) {
	//		throw std::runtime_error("Failed to open mesh file: " + file);
	//	}

	//	// Read grid width and height
	//	int width, height;
	//	ifs >> width >> height;
	//	if (!ifs) {
	//		throw std::runtime_error("Failed to read grid dimensions from file: " + file);
	//	}

	//	// Create a grid to store tile information
	//	grid.resize(height, std::vector<int>(width));

	//	// Save each tile ID into the grid
	//	for (int row = 0; row < height; ++row) {
	//		for (int col = 0; col < width; ++col) {
	//			if (!(ifs >> grid[row][col])) {
	//				throw std::runtime_error("Failed to read tile data at row " + std::to_string(row) + ", column " + std::to_string(col));
	//			}
	//		}
	//	}

	//	// Calculate offset to center the grid
	//	float tile_size = 100.0f;
	//	float offset_x = (width * tile_size) / 2.0f - center.x;
	//	float offset_y = (height * tile_size) / 2.0f - center.y;

	//	// Create entities from grid info
	//	for (int row = 0; row < height; ++row) {
	//		for (int col = 0; col < width; ++col) {
	//			// Create tile
	//			createTile(grid[row][col], row, col, tile_size, offset_x, offset_y, width, height, background_layer, player_layer);
	//		}
	//	}
	//}

	//void Serialization::Service::createTile(int tileID, int row, int col, float tile_size, float offset_x, float offset_y, int width, int height,
	//	std::shared_ptr<NIKE::Scenes::Layer>& background_layer,
	//	std::shared_ptr<NIKE::Scenes::Layer>& player_layer) {
	//	bool flip{ false };
	//	bool collide{ false };
	//	std::string texture_name{};

	//	switch (tileID) {
	//	case 1: texture_name = "wallTopCorner"; collide = true; break;
	//	case 2: texture_name = "wallTopMiddle"; collide = true; break;
	//	case 3: texture_name = "wallLeft"; collide = true; break;
	//	case 4: texture_name = "grass"; break;
	//	case 5: texture_name = "wallBottomCorner"; collide = true; break;
	//	case 6: texture_name = "wallBottomMiddle"; collide = true; break;
	//	case 7: texture_name = "wallTopCorner"; flip = true; collide = true; break;
	//	case 8: texture_name = "wallLeft"; flip = true; collide = true; break;
	//	case 9: texture_name = "wallBottomCorner"; flip = true; collide = true; break;
	//	default: texture_name = "grass"; break;
	//	}

	//	// Set layer to background if not collidable
	//	unsigned int layer = (collide) ? player_layer->getLayerID() : background_layer->getLayerID();
	//	NIKE::Entity::Type tile_entity = NIKE_ECS_SERVICE->createEntity(layer);

	//	NIKE_IMGUI_SERVICE->addEntityRef("tile_" + std::to_string(row) + "_" + std::to_string(col), tile_entity);
	//	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(tile_entity,
	//		NIKE::Transform::Transform({ col * tile_size - offset_x, (height - 1 - row) * tile_size - offset_y },
	//			{ 100.0f, 100.0f }, 0.0f));

	//	if (collide) {
	//		NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Dynamics>(tile_entity, NIKE::Physics::Dynamics(200.0f, 1.0f, 0.1f));
	//		NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Collider>(tile_entity, NIKE::Physics::Collider(NIKE::Physics::Resolution::NONE));
	//	}

	//	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(tile_entity, NIKE::Render::Texture(texture_name,
	//		{ 1.0f, 1.0f, 1.0f, 1.0f }, false, 0.5f, false, { 1, 1 }, { 0, 0 }, { flip, false }));
	//}
}

