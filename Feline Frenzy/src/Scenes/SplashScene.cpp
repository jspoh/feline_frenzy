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
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadShader("base", "assets/Shaders/base.vert", "assets/Shaders/base.frag");
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadShader("texture", "assets/Shaders/texture.vert", "assets/Shaders/texture.frag");

	//Load Models
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadModel("square", "assets/Models/square.txt");
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadModel("TRIANGLE", "assets/Models/triangle.txt");
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadModel("CIRCLE", "assets/Models/circle.txt");
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadModel("square-texture", "assets/Models/square-texture.txt");

	//Load Texture
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadTexture("PLAYER", "assets/Textures/player.png");
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadTexture("TREE", "assets/Textures/Tree_Orange.png");
	//Load Font
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadFont("MONTSERRAT", "assets/Fonts/Montserrat-Bold.ttf");

	//Load music
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadMusic("MUSIC", "assets/Audios/test_music.wav");
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadSound("SFX", "assets/Audios/test_sound.wav");
}

void Splash::Scene::init() {

	std::shared_ptr<NIKESAURUS::Scenes::Layer> base_Layer = registerLayer("BASE");
	std::shared_ptr<NIKESAURUS::Scenes::Layer> second_layer = registerLayer("SECOND");

	//Creat Channel Group
	NIKEEngine.getService<NIKESAURUS::Audio::Service>()->createChannelGroup("MASTER");

	//Play Music
	NIKEEngine.getService<NIKESAURUS::Audio::Service>()->playAudio("MUSIC", "MUSIC_CHANNEL", "MASTER", 1.0f, 1.0f, true);

	//Set Music Volume
	NIKEEngine.getService<NIKESAURUS::Audio::Service>()->getChannelGroup("MASTER")->setVolume(0.5f);

	// Test Logger
	NIKEE_WARN("This is a warning message");

	//Create player
	NIKESAURUS::Entity::Type player_1 = NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->createEntity();
	NIKEEngine.getService<NIKESAURUS::IMGUI::Service>()->addEntityRef("PLAYER", player_1);
	second_layer->addEntity(player_1);
	NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Transform::Transform>(player_1, NIKESAURUS::Transform::Transform({0.0f, 0.0f}, {100.0f, 100.0f}, 0.0f));
	NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Transform::Velocity>(player_1, NIKESAURUS::Transform::Velocity());
	NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Render::Texture>(player_1, NIKESAURUS::Render::Texture("PLAYER", {0.0f, 0.0f, 0.0f, 1.0f}));
	NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Render::Cam>(player_1, NIKESAURUS::Render::Cam(NIKEEngine.getService<NIKESAURUS::Windows::Service>()->getWindow()->getWindowSize().y));

	NIKEEngine.getService<NIKESAURUS::Events::Service>()->dispatchEvent(std::make_shared<NIKESAURUS::Render::ChangeCamEvent>(player_1));

	NIKESAURUS::Entity::Type shape_1 = NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->createEntity();
	NIKEEngine.getService<NIKESAURUS::IMGUI::Service>()->addEntityRef("shape_1", shape_1);
	base_Layer->addEntity(shape_1);
	//NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addDefEntityComponent(shape_1, NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->getAllComponentTypes().at("Render::Texture"));
	NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Transform::Transform>(shape_1, NIKESAURUS::Transform::Transform({0.0f, 0.0f}, {100.0f, 100.0f}, 0.0f));
	NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Render::Texture>(shape_1, NIKESAURUS::Render::Texture("TREE", {0.0f, 0.0f, 0.0f, 1.0f}));
	
	//Method 1
	 //NIKESAURUS::Entity::Type sfx_1 = NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->createEntity();
	 //NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Audio::SFX>(sfx_1, NIKESAURUS::Audio::SFX(true, "SFX", "MASTER", 0.5f, 1.0f));

	NIKESAURUS::Entity::Type sfx_1 = NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->createEntity();
	base_Layer->addEntity(sfx_1);
	NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addDefEntityComponent(sfx_1, NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->getAllComponentTypes().at("Audio::SFX"));
	NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->getEntityComponent<NIKESAURUS::Audio::SFX>(sfx_1) = { true, "SFX", "MASTER", 0.5f, 1.0f };
	
	// Test crash logger
	//LOG_CRASH("This is a test crash");
}

void Splash::Scene::exit() {

}

void Splash::Scene::unload() {

}
