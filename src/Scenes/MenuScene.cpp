/*****************************************************************//**
 * \file   MenuScene.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/MenuScene.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Systems/sysPhysics.h"

//!TODO Clean up scene parser
void Menu::Scene::loadFromFile(const std::string& scene_filepath, std::vector<Entity::Type>& entities) {
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
		//!TODO add data serialization for textured object
		NIKEEngine.addEntityComponentObj<Render::Mesh>(entity, { shdr_prgm, model_name, Matrix33::Matrix_33()});
		NIKEEngine.addEntityComponentObj<Transform::Transform>(entity, { position, scaling, 0.0f }); // 0.0f for rotation
		NIKEEngine.addEntityComponentObj<Render::Color>(entity, { clr_atr, 1.0f });  // Assuming alpha = 1.0f for now


		// Push the entity into the vector
		entities.push_back(entity);
	}
}

void Menu::Scene::load() {
	//obj->setColor(Vector3(0.5f, 0.5f, 0.5f));
	NIKEEngine.registerComponent<Transform::Velocity>();
	NIKEEngine.registerComponent<Transform::Transform>();
	NIKEEngine.registerComponent<Transform::Runtime_Transform>();
	NIKEEngine.registerComponent<Move::Movement>();
	NIKEEngine.registerComponent<Render::Mesh>();
	NIKEEngine.registerComponent<Render::Color>();
	NIKEEngine.registerComponent<Render::Cam>();

	//Add Singleton System
	NIKEEngine.registerSystem<Render::Manager>(Render::Manager::getInstance());
	NIKEEngine.registerSystem<Physics::Manager>(Physics::Manager::getInstance());

	// Set components link
	// NIKEEngine.accessSystem<Render::Manager>()->setComponentsLinked(false);
	NIKEEngine.accessSystem<Physics::Manager>()->setComponentsLinked(false);

	

	//Add component types to system
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Transform::Transform>());
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Transform::Runtime_Transform>());
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Render::Mesh>());
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Render::Color>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Transform::Velocity>());
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Move::Movement>());
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Transform::Runtime_Transform>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Move::Movement>());

	//Load models
	NIKEEngine.accessSystem<Render::Manager>()->registerModel("square", "assets/meshes/square.txt");
	NIKEEngine.accessSystem<Render::Manager>()->registerModel("triangle", "assets/meshes/triangle.txt");
	NIKEEngine.accessSystem<Render::Manager>()->registerModel("circle", "assets/meshes/circle.txt");
	NIKEEngine.accessSystem<Render::Manager>()->registerModel("square-texture", "assets/meshes/square-texture.txt");

	// load textures
	NIKEEngine.accessSystem<Render::Manager>()->registerTexture("duck", "assets/textures/duck-rgba-256.tex");
	NIKEEngine.accessSystem<Render::Manager>()->registerTexture("water", "assets/textures/water-rgba-256.tex");
	NIKEEngine.accessSystem<Render::Manager>()->registerTexture("tree", "assets/textures/tree.jpg");
}

void Menu::Scene::init() {
	glClearColor(1, 1, 1, 1);

	//Create entity
	std::vector<Entity::Type> entities;

	loadFromFile("assets/scenes/mainmenu.scn", entities);

	NIKEEngine.addEntityComponentObj<Move::Movement>(entities.at(1), {false, false, false, false});
	
	NIKEEngine.addEntityComponentObj<Transform::Velocity>(entities.at(2), { {0.0f, 1.0f} });

	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Render::Mesh>(entities.at(3), { "tex", "square-texture", Matrix33::Matrix_33::Identity(), "duck" });
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(3), { {400.0f, -200.0f}, {200.f, 200.f}, 0.0f });
	NIKEEngine.addEntityComponentObj<Transform::Runtime_Transform>(entities.at(3), Transform::Runtime_Transform());
	NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(3), { {1.0f, 1.0f, 1.0f}, 1.0f });

	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Render::Mesh>(entities.at(4), { "tex", "square-texture", Matrix33::Matrix_33::Identity(), "water" });
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(4), { {100.0f, -200.0f}, {200.f, 200.f}, 0.0f });
	NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(4), { {1.0f, 1.0f, 1.0f}, 1.0f });

	entities.push_back(NIKEEngine.createEntity());
	NIKEEngine.addEntityComponentObj<Render::Mesh>(entities.at(5), { "tex", "square-texture", Matrix33::Matrix_33::Identity(), "tree" });
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(5), { {-200.0f, -200.0f}, {200.f, 200.f}, 0.0f });
	NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(5), { {1.0f, 1.0f, 1.0f}, 1.0f });
	NIKEEngine.addEntityComponentObj<Transform::Velocity>(entities.at(1), { {0.0f, 0.0f} });


	//entities.push_back(NIKEEngine.createEntity());
	//NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(3), Transform::Transform());
	//NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(3), Render::Color());

	NIKEEngine.addEntityComponentObj<Render::Cam>(entities.at(0), { "CAM1", {0.0f, 0.0f}, 1000.0f });
	NIKEEngine.addEntityComponentObj<Render::Cam>(entities.at(1), { "CAM2", {122.0f, 0.0f}, 1000.0f });
	NIKEEngine.accessSystem<Render::Manager>()->trackCamEntity("CAM2");

	//entities.push_back(NIKEEngine.createEntity());
	//NIKEEngine.addEntityComponentObj<Render::Mesh>(entities.at(2), { "base", "circle", Matrix33::Matrix_33() });
	//NIKEEngine.addEntityComponentObj<Transform::Transform>(entities.at(2), { {322.0f, 122.0f}, {100.f, 100.f}, 0.0f });
	//NIKEEngine.addEntityComponentObj<Render::Color>(entities.at(2), { {1.0f, 0.0f, 1.0f}, 1.0f });

}

void Menu::Scene::exit() {

}

void Menu::Scene::unload() {

}
