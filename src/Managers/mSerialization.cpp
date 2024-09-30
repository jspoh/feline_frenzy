/*****************************************************************//**
 * \file   mSerialization.cpp
 * \brief  Serialization manager
 *
 * \author b.soh
 * \date   September 2024
 *********************************************************************/
#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mSerialization.h"

void Serialization::Manager::loadSceneFromFile(const std::string& scene_filepath, std::vector<Entity::Type>& entities) {
	std::ifstream ifs{ scene_filepath, std::ios::in };

	// Open Scene file
	if (!ifs) {
		cerr << "Failed to open mesh file: " << scene_filepath << endl;
		return;
	}

	ifs.seekg(0, std::ios::beg);
	std::string line;
	getline(ifs, line); // first line is count of objects in scene
	std::istringstream line_sstm{ line };
	int obj_cnt;
	line_sstm >> obj_cnt; // read count of objects in scene

	while (obj_cnt--) { // read each object's parameters
		// Create entity
		Entity::Type entity = NIKEEngine.createEntity();

		std::string model_name, object_name, shdr_prgm, vert_file, frag_file;
		Vector3 clr_atr{};
		Vector2 scaling{}, orientation{}, position{};

		// !TODO create a proper system to register mesh data
		getline(ifs, line); // 1st parameter: model's name
		std::istringstream line_modelname{ line };
		line_modelname >> model_name;

		getline(ifs, line); // object name
		std::istringstream line_objectname{ line };
		line_objectname >> object_name;

		// !TODO create a proper system to register shader files
		getline(ifs, line); // shader files
		std::istringstream line_shdrprgm{ line };
		line_shdrprgm >> shdr_prgm >> vert_file >> frag_file;

		// Color
		getline(ifs, line);
		std::istringstream line_clrattr{ line };
		line_clrattr >> clr_atr.x >> clr_atr.y >> clr_atr.z;

		// Scaling
		getline(ifs, line);
		std::istringstream line_scale{ line };
		line_scale >> scaling.x >> scaling.y;

		// Orientation & rotation
		getline(ifs, line);
		std::istringstream line_orient{ line };
		line_orient >> orientation.x >> orientation.y;

		// Position
		getline(ifs, line);
		std::istringstream line_pos{ line };
		line_pos >> position.x >> position.y;

		// Add components to the entity
		NIKEEngine.addEntityComponentObj<Render::Mesh>(entity, { shdr_prgm, model_name, Matrix33::Matrix_33() });
		NIKEEngine.addEntityComponentObj<Transform::Transform>(entity, { position, scaling, 0.0f }); // 0.0f for rotation
		NIKEEngine.addEntityComponentObj<Render::Color>(entity, { clr_atr, 1.0f });  // Assuming alpha = 1.0f for now

		// Push the entity into the vector
		entities.push_back(entity);
	}
}