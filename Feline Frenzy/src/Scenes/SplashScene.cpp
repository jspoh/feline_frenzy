///*****************************************************************//**
// * \file   SplashScene.cpp
// * \brief
// * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (90%)
// * \co-author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (10%)
// * \date   September 2024
// * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
// *********************************************************************/

#include "Core/pch.h"
#include "Scenes/SplashScene.h"


void loadBackgroundFromFile(const std::string& file, std::shared_ptr<NIKE::Scenes::Layer>& layer, std::vector<std::vector<int>>& grid, const NIKE::Math::Vector2<float>& center) {
	// Open Scene file
	std::ifstream ifs{ file, std::ios::in };

	if (!ifs) {
		throw std::runtime_error("Failed to open mesh file: " + file);
	}

	// Read grid width and height
	int width, height;
	ifs >> width >> height;

	if (!ifs) {
		throw std::runtime_error("Failed to read grid dimensions from file: " + file);
	}

	// Create a grid to store tile information
	grid.resize(height, std::vector<int>(width));

	// Save each tile ID into the grid
	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			if (!(ifs >> grid[row][col])) {
				throw std::runtime_error("Failed to read tile data at row " + std::to_string(row) + ", column " + std::to_string(col));
			}
		}
	}

	//cout << "Grid loaded successfully. Data:" << endl;

	// Calculate offset to center the grid
	float tile_size = 100.0f;
	float offset_x = (width * tile_size) / 2.0f - center.x;  // Centering the x-axis
	float offset_y = (height * tile_size) / 2.0f - center.y; // Centering the y-axis

	// Create entities from grid info
	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			int tileID = grid[row][col];

			//cout << grid[row][col] << " ";

			
			// Create tile here
			bool flip{ false };
			bool collide{ false };
			std::string texture_name{};
			switch (tileID) {
			case 1:
				texture_name = "wallTopCorner";
				flip = false;
				collide = true;
				break;
			case 2:
				texture_name = "wallTopMiddle";
				flip = false;
				collide = true;
				break;
			case 3:
				texture_name = "wallLeft";
				flip = false;
				collide = true;
				break;
			case 4:
				texture_name = "grass";
				flip = false;
				collide = false;
				break;
			case 5:
				texture_name = "wallBottomCorner";
				flip = false;
				collide = true;
				break;
			case 6:
				texture_name = "wallBottomMiddle";
				flip = false;
				collide = true;
				break;
			case 7:
				texture_name = "wallTopCorner";
				flip = true;
				collide = true;
				break;
			case 8:
				texture_name = "wallLeft";
				flip = true;
				collide = true;
				break;
			case 9:
				texture_name = "wallBottomCorner";
				flip = true;
				collide = true;
				break;
			default:
				texture_name = "grass";
				flip = false;
				collide = false;
				break;
			}

			// Create Entity
			NIKE::Entity::Type tile_entity = NIKE_ECS_SERVICE->createEntity();
			NIKE_IMGUI_SERVICE->addEntityRef("tile_" + std::to_string(row) + "_" + std::to_string(col), tile_entity);
			layer->addEntity(tile_entity);
			NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(tile_entity, NIKE::Transform::Transform({ col * tile_size - offset_x, (height - 1 - row) * tile_size - offset_y }, { 100.0f, 100.0f }, 0.0f));
			
			
			//if (collide) {
			//	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Dynamics>(tile_entity, NIKE::Physics::Dynamics(200.0f, 1.0f, 0.1f));
			//	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Collider>(tile_entity, NIKE::Physics::Collider(NIKE::Physics::Resolution::NONE));
			//}

			//if (flip) {
			//	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(tile_entity, NIKE::Render::Texture(texture_name, { 1.0f, 1.0f, 1.0f, 1.0f }, false, 0.5f, false, { 1, 1 }, { 0, 0 }, { true, false }));
			//}
			//else {
			//	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(tile_entity, NIKE::Render::Texture(texture_name, { 1.0f, 1.0f, 1.0f, 1.0f }, false, 0.5f, false, { 1, 1 }, { 0, 0 }, { false, false }));
			//}
		}
		//cout << endl;
	}
	//cout << "Loaded background grid from file successfully." << endl;
}



void Splash::Scene::load() {

	//Load Shaders
	NIKE_ASSETS_SERVICE->loadShader("base", "assets/Shaders/base.vert", "assets/Shaders/base.frag");
	NIKE_ASSETS_SERVICE->loadShader("texture", "assets/Shaders/texture.vert", "assets/Shaders/texture.frag");
	NIKE_ASSETS_SERVICE->loadShader("text", "assets/Shaders/text.vert", "assets/Shaders/text.frag");

	//Load Models
	NIKE_ASSETS_SERVICE->loadModel("square", "assets/Models/square.txt");
	NIKE_ASSETS_SERVICE->loadModel("TRIANGLE", "assets/Models/triangle.txt");
	NIKE_ASSETS_SERVICE->loadModel("CIRCLE", "assets/Models/circle.txt");
	NIKE_ASSETS_SERVICE->loadModel("square-texture", "assets/Models/square-texture.txt");

	//Load Texture
	NIKE_ASSETS_SERVICE->loadTexture("PLAYER", "assets/Textures/player.png");
	NIKE_ASSETS_SERVICE->loadTexture("TREE", "assets/Textures/Tree_Orange.png");
	NIKE_ASSETS_SERVICE->loadTexture("ZOMBIE", "assets/Textures/ZombieSheet.png");
	NIKE_ASSETS_SERVICE->loadTexture("wallTopCorner", "assets/Textures/TopCorner.png");
	NIKE_ASSETS_SERVICE->loadTexture("wallTopMiddle", "assets/Textures/M1.png");
	NIKE_ASSETS_SERVICE->loadTexture("wallLeft", "assets/Textures/Left.png");
	NIKE_ASSETS_SERVICE->loadTexture("grass", "assets/Textures/Grass.png");
	NIKE_ASSETS_SERVICE->loadTexture("wallBottomCorner", "assets/Textures/BtmCorner.png");
	NIKE_ASSETS_SERVICE->loadTexture("wallBottomMiddle", "assets/Textures/BtmMiddle.png");

	//Load Font
	NIKE_ASSETS_SERVICE->loadFont("MONTSERRAT", "assets/Fonts/Montserrat-Bold.ttf");

	//Load music
	NIKE_ASSETS_SERVICE->loadMusic("MUSIC", "assets/Audios/test_music.wav");
	NIKE_ASSETS_SERVICE->loadSound("SFX", "assets/Audios/test_sound.wav");
}

void Splash::Scene::init() {
	//std::shared_ptr<NIKE::Scenes::Layer> background_layer = registerLayer("BACKGROUND");
	std::shared_ptr<NIKE::Scenes::Layer> base_layer = registerLayer("BASE");
	std::shared_ptr<NIKE::Scenes::Layer> second_layer = registerLayer("SECOND");

	//Creat Channel Group
	NIKE_AUDIO_SERVICE->createChannelGroup("MASTER");

	//Play Music
	NIKE_AUDIO_SERVICE->playAudio("MUSIC", "MUSIC_CHANNEL", "MASTER", 1.0f, 1.0f, true);

	//Set Music Volume
	NIKE_AUDIO_SERVICE->getChannelGroup("MASTER")->setVolume(0.5f);

	// Test Logger
	NIKEE_WARN("This is a warning message");

	NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Physics::ChangePhysicsEvent>(1.0f));

	//Create player
	NIKE::Entity::Type player_1 = NIKE_ECS_SERVICE->createEntity();
	NIKE_IMGUI_SERVICE->addEntityRef("player_1", player_1);
	second_layer->addEntity(player_1);
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(player_1, NIKE::Transform::Transform({0.0f, 200.0f}, {100.0f, 100.0f}, 0.0f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Dynamics>(player_1, NIKE::Physics::Dynamics(200.0f, 1.0f, 2.0f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Collider>(player_1, NIKE::Physics::Collider(NIKE::Physics::Resolution::SLIDE));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(player_1, NIKE::Render::Texture("ZOMBIE", {1.0f, 0.0f, 0.0f, 1.0f}, true, 0.5f, false, {9, 5}, {0, 0}, {false, true}));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Cam>(player_1, NIKE::Render::Cam(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Animation::Base>(player_1, NIKE::Animation::Base(0, 0.2f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Animation::Sprite>(player_1, NIKE::Animation::Sprite({9,5}, {0, 1}, {8, 1}));
	NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::ChangeCamEvent>(player_1));

	// MapGrid Test
	std::vector<std::vector<int>> grid;
	loadBackgroundFromFile("assets/Map/mediummap.txt", base_layer, grid, { 0.0f, 200.0f });

	// TREE
	//Save player to prefab
	NIKE_SERIALIZE_SERVICE->saveEntityToFile(player_1, "assets/Scenes/test.scn");

	NIKE::Entity::Type shape_1 = NIKE_ECS_SERVICE->createEntity();
	NIKE_IMGUI_SERVICE->addEntityRef("shape_1", shape_1);
	second_layer->addEntity(shape_1);
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Cam>(shape_1, NIKE::Render::Cam(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(shape_1, NIKE::Transform::Transform({0.0f, 0.0f}, {100.0f, 100.0f}, 0.0f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Dynamics>(shape_1, NIKE::Physics::Dynamics(200.0f, 1.0f, 0.1f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Collider>(shape_1, NIKE::Physics::Collider(NIKE::Physics::Resolution::NONE));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(shape_1, NIKE::Render::Texture("TREE", {1.0f, 1.0f, 1.0f, 1.0f}));

	NIKE::Entity::Type shape_2 = NIKE_ECS_SERVICE->createEntity();
	NIKE_IMGUI_SERVICE->addEntityRef("shape_2", shape_2);
	base_layer->addEntity(shape_2);
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Cam>(shape_2, NIKE::Render::Cam(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(shape_2, NIKE::Transform::Transform({ 200.0f, 100.0f }, { 100.0f, 100.0f }, 0.0f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(shape_2, NIKE::Render::Texture("TREE", { 1.0f, 1.0f, 1.0f, 1.0f }));

	
	NIKE::Entity::Type text_1 = NIKE_ECS_SERVICE->createEntity();
	NIKE_IMGUI_SERVICE->addEntityRef("text_1", text_1);
	second_layer->addEntity(text_1);
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(text_1, NIKE::Transform::Transform({ 0.0f, -300.0f }, { 0.0f, 0.0f }, 45.0f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Text>(text_1, NIKE::Render::Text("MONTSERRAT", "HELLO WORLD.", {1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, NIKE::Render::TextOrigin::CENTER));

	NIKE::Entity::Type text_2 = NIKE_ECS_SERVICE->createEntity();
	NIKE_IMGUI_SERVICE->addEntityRef("text_2", text_2);
	second_layer->addEntity(text_2);
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(text_2, NIKE::Transform::Transform({ -750.0f, 400.0f }, { 0.0f, 0.0f }, 0.0f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Text>(text_2, NIKE::Render::Text("MONTSERRAT", "PANTAT.", { 1.0f, 0.0f, 0.0f, 1.0f }, 1.0f, NIKE::Render::TextOrigin::LEFT));

	NIKE::Entity::Type sfx_1 = NIKE_ECS_SERVICE->createEntity();
	base_layer->addEntity(sfx_1);
	NIKE_ECS_SERVICE->addDefEntityComponent(sfx_1, NIKE_ECS_SERVICE->getAllComponentTypes().at("Audio::SFX"));
	NIKE_ECS_SERVICE->getEntityComponent<NIKE::Audio::SFX>(sfx_1) = { true, "SFX", "MASTER", 0.5f, 1.0f };

	//Prefabs testing
	NIKE::Entity::Type test_1 = NIKE_ECS_SERVICE->createEntity();
	second_layer->addEntity(test_1);

	NIKE_SERIALIZE_SERVICE->loadEntityFromFile(test_1, "assets/Scenes/test.scn");
	NIKE_ECS_SERVICE->getEntityComponent<NIKE::Physics::Dynamics>(test_1).force.x = 100.0f;
}

void Splash::Scene::exit() {

}

void Splash::Scene::unload() {

}