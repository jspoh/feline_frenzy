/*****************************************************************//**
 * \file   mSerialization.cpp
 * \brief  Serialization manager
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   September 2024
 *********************************************************************/
#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mSerialization.h"
#include "../headers/Core/Engine.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Systems/Physics/sysPhysics.h"
#include "../headers/Components/cMove.h"

void Serialization::Manager::loadSceneFromFile(const std::string& scene_filepath, std::unordered_map<std::string, Entity::Type>& entities) {
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


		std::string model_name, mesh_file, object_name, shdr_prgm, tex, vert_file, frag_file;
		Vector3 clr_atr{};
		Vector2 scaling{}, orientation{}, velocity{}, position{};

		getline(ifs, line); // 1st parameter: model's name
		std::istringstream line_modelname{ line };
		line_modelname >> model_name;

		// Register model
		getline(ifs, line); // object name
		std::istringstream line_objectname{ line };
		line_objectname >> object_name;

		// !TODO create a proper system to register shader files
		getline(ifs, line); // shader files
		std::istringstream line_shdrprgm{ line };
		line_shdrprgm >> shdr_prgm >> tex ;

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

		// Velocity
		getline(ifs, line);
		std::istringstream line_vel{ line };
		line_vel >> velocity.x >> velocity.y;

		// Position
		getline(ifs, line);
		std::istringstream line_pos{ line };
		line_pos >> position.x >> position.y;

		if (entities.find(object_name) != entities.end()) {
			cerr << "Object name already exists! skipping entity: " << object_name << endl;
			continue;
		}

		// Add components to the entity
		// Create entity
		Entity::Type entity = NIKEEngine.createEntity();

		if (shdr_prgm == "base") {
			NIKEEngine.addEntityComponentObj<Render::Shape>(entity, { model_name, Matrix33::Matrix_33(), { clr_atr, 1.0f } });
		}
		else if (shdr_prgm == "texture") {
			NIKEEngine.addEntityComponentObj<Render::Texture>(entity, { tex ,Matrix33::Matrix_33::Identity(), { clr_atr, 1.0f }, { 1.0f, 1.0f}, {0.0f, 0.0f} });
		}
		
		NIKEEngine.addEntityComponentObj<Transform::Velocity>(entity, { {velocity.x, velocity.y} });
		NIKEEngine.addEntityComponentObj<Transform::Transform>(entity, { position, scaling, orientation.x });

		// Set collider, currently checks if object name has "bounce" substring to enable bouncing on collision // Added by MKK
		if (object_name.find("bounce") != std::string::npos) {
			// The substring "bounce" is found in object_name
			NIKEEngine.accessSystem<Physics::Manager>()->collision_manager.setColliderComp(entity, true);
		}
		else if (object_name.find("background") != std::string::npos)
		{
			 // Not to add collision to background texture
		}
		else {
			NIKEEngine.accessSystem<Physics::Manager>()->collision_manager.setColliderComp(entity);
		}
		

		// Init Player Camera (ALL SCENE MUST HAVE)
		if (object_name == "player") {
			NIKEEngine.addEntityComponentObj<Input::Key>(entity, { Input::TriggerMode::CHECKED });
			NIKEEngine.addEntityComponentObj<Render::Cam>(entity, { object_name, {0.0f, 0.0f}, NIKEEngine.accessWindow()->getWindowSize().y });
			NIKEEngine.addEntityComponentObj<Move::Movement>(entity, { false, false, false, false });
			NIKEEngine.accessSystem<Render::Manager>()->trackCamEntity(object_name);
		}


		// Map entity into map
		entities[object_name] = entity;
	}
}