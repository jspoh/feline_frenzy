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

void Splash::Scene::load() {

	//Load Shaders
	NIKE_ASSETS_SERVICE->loadShader("base", "assets/Shaders/base.vert", "assets/Shaders/base.frag");
	NIKE_ASSETS_SERVICE->loadShader("texture", "assets/Shaders/texture.vert", "assets/Shaders/texture.frag");
	NIKE_ASSETS_SERVICE->loadShader("text", "assets/Shaders/text.vert", "assets/Shaders/text.frag");
	NIKE_ASSETS_SERVICE->loadShader("batched_base", "assets/Shaders/batched_base.vert", "assets/Shaders/batched_base.frag");

	//Load Models
	NIKE_ASSETS_SERVICE->loadModel("square", "assets/Models/square.txt");
	NIKE_ASSETS_SERVICE->loadModel("TRIANGLE", "assets/Models/triangle.txt");
	NIKE_ASSETS_SERVICE->loadModel("CIRCLE", "assets/Models/circle.txt");
	NIKE_ASSETS_SERVICE->loadModel("square-texture", "assets/Models/square-texture.txt");

	//Load Texture
	NIKE_ASSETS_SERVICE->loadTexture("PLAYER", "assets/Textures/player.png");
	NIKE_ASSETS_SERVICE->loadTexture("TREE", "assets/Textures/Tree_Orange.png");
	NIKE_ASSETS_SERVICE->loadTexture("ZOMBIE", "assets/Textures/ZombieSheet.png");

	//Load Font
	NIKE_ASSETS_SERVICE->loadFont("MONTSERRAT", "assets/Fonts/Montserrat-Bold.ttf");

	//Load music
	NIKE_ASSETS_SERVICE->loadMusic("MUSIC", "assets/Audios/test_music.wav");
	NIKE_ASSETS_SERVICE->loadSound("SFX", "assets/Audios/test_sound.wav");
}

void Splash::Scene::init() {

	std::shared_ptr<NIKE::Scenes::Layer> base_Layer = registerLayer("BASE");
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
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(player_1, NIKE::Transform::Transform({ 0.0f, 200.0f }, { 100.0f, 100.0f }, 0.0f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Dynamics>(player_1, NIKE::Physics::Dynamics(200.0f, 1.0f, 2.0f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Collider>(player_1, NIKE::Physics::Collider(NIKE::Physics::Resolution::BOUNCE));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(player_1, NIKE::Render::Texture("ZOMBIE", {1.0f, 0.0f, 0.0f, 1.0f}, true, 0.5f, false, {9, 5}, {0, 0}, {false, true}));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Cam>(player_1, NIKE::Render::Cam(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Animation::Base>(player_1, NIKE::Animation::Base(0, 0.2f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Animation::Sprite>(player_1, NIKE::Animation::Sprite({ 9,5 }, { 0, 1 }, { 8, 1 }));

	NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::ChangeCamEvent>(player_1));

	NIKE::Entity::Type shape_1 = NIKE_ECS_SERVICE->createEntity();
	NIKE_IMGUI_SERVICE->addEntityRef("shape_1", shape_1);
	second_layer->addEntity(shape_1);
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(shape_1, NIKE::Transform::Transform({ 0.0f, 0.0f }, { 100.0f, 100.0f }, 0.0f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Dynamics>(shape_1, NIKE::Physics::Dynamics(200.0f, 1.0f, 0.1f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Collider>(shape_1, NIKE::Physics::Collider(NIKE::Physics::Resolution::NONE));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(shape_1, NIKE::Render::Texture("TREE", { 1.0f, 1.0f, 1.0f, 1.0f }));

	NIKE::Entity::Type text_1 = NIKE_ECS_SERVICE->createEntity();
	NIKE_IMGUI_SERVICE->addEntityRef("text_1", text_1);
	second_layer->addEntity(text_1);
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(text_1, NIKE::Transform::Transform({ 0.0f, -300.0f }, { 0.0f, 0.0f }, 45.0f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Text>(text_1, NIKE::Render::Text("MONTSERRAT", "HELLO WORLD.", { 1.0f, 0.0f, 0.0f, 1.0f }, 1.0f, NIKE::Render::TextOrigin::CENTER));

	NIKE::Entity::Type text_2 = NIKE_ECS_SERVICE->createEntity();
	NIKE_IMGUI_SERVICE->addEntityRef("text_2", text_2);
	second_layer->addEntity(text_2);
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(text_2, NIKE::Transform::Transform({ -750.0f, 400.0f }, { 0.0f, 0.0f }, 0.0f));
	NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Text>(text_2, NIKE::Render::Text("MONTSERRAT", "PANTAT.", { 1.0f, 0.0f, 0.0f, 1.0f }, 1.0f, NIKE::Render::TextOrigin::LEFT));

	NIKE::Entity::Type sfx_1 = NIKE_ECS_SERVICE->createEntity();
	base_Layer->addEntity(sfx_1);
	NIKE_ECS_SERVICE->addDefEntityComponent(sfx_1, NIKE_ECS_SERVICE->getAllComponentTypes().at("Audio::SFX"));
	NIKE_ECS_SERVICE->getEntityComponent<NIKE::Audio::SFX>(sfx_1) = { true, "SFX", "MASTER", 0.5f, 1.0f };
	// Test crash logger
	//LOG_CRASH("This is a test crash");

	// batch rendering test (shapes)
	//NIKE::Entity::Type batch_shape_1 = NIKE_ECS_SERVICE->createEntity();
	//NIKE_IMGUI_SERVICE->addEntityRef("batch_shape_1", batch_shape_1);
	//second_layer->addEntity(batch_shape_1);
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(batch_shape_1, NIKE::Transform::Transform({ 100.0f, 200.0f }, { 100.0f, 100.0f }, 0.0f));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Shape>(batch_shape_1, { "square", {1, 0, 0, 1}, {0, 0} });

	//NIKE::Entity::Type batch_shape_2 = NIKE_ECS_SERVICE->createEntity();
	//NIKE_IMGUI_SERVICE->addEntityRef("batch_shape_2", batch_shape_2);
	//second_layer->addEntity(batch_shape_2);
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(batch_shape_2, NIKE::Transform::Transform({ -100.0f, 200.0f }, { 100.0f, 100.0f }, 0.0f));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Shape>(batch_shape_2, { "square", {0, 1, 0, 1}, {0, 0} });

	// 1000 objects at 3fps (without batch rendering on jspoh's computer)
	constexpr int BATCH_RENDERING_SHAPES_COUNT = 1015;
	srand(time(NULL));

	for (int i{}; i < BATCH_RENDERING_SHAPES_COUNT; i++) {
		NIKE::Entity::Type batch_shape = NIKE_ECS_SERVICE->createEntity();
		std::stringstream ss;
		ss << "batch_shape_" << i;
		NIKE_IMGUI_SERVICE->addEntityRef(ss.str(), batch_shape);
		second_layer->addEntity(batch_shape);

		const Vector2i window_size = NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize() / 2.f;
		const float RAND_X = (rand() % window_size.x) - (window_size.x / 2.f);
		const float RAND_Y = (rand() % window_size.y) - (window_size.y / 2.f);

		const float RAND_R = (rand() & 100) / 100.f;
		const float RAND_G = (rand() & 100) / 100.f;
		const float RAND_B = (rand() & 100) / 100.f;

		NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(batch_shape, NIKE::Transform::Transform({ RAND_X, RAND_Y }, { 100.0f, 100.0f }, 0.0f));
		NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Shape>(batch_shape, { "square", {RAND_R, RAND_G, RAND_B, 1}, {0, 0} });
	}
}

void Splash::Scene::exit() {

}

void Splash::Scene::unload() {

}
