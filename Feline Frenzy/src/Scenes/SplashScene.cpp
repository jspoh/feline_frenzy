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
	NIKE_ENGINE.getService<NIKESAURUS::Assets::Service>()->loadShader("base", "assets/Shaders/base.vert", "assets/Shaders/base.frag");
	NIKE_ENGINE.getService<NIKESAURUS::Assets::Service>()->loadShader("texture", "assets/Shaders/texture.vert", "assets/Shaders/texture.frag");

	//Load Models
	NIKE_ENGINE.getService<NIKESAURUS::Assets::Service>()->loadModel("square", "assets/Models/square.txt");
	NIKE_ENGINE.getService<NIKESAURUS::Assets::Service>()->loadModel("TRIANGLE", "assets/Models/triangle.txt");
	NIKE_ENGINE.getService<NIKESAURUS::Assets::Service>()->loadModel("CIRCLE", "assets/Models/circle.txt");
	NIKE_ENGINE.getService<NIKESAURUS::Assets::Service>()->loadModel("square-texture", "assets/Models/square-texture.txt");

	//Load Texture
	NIKE_ENGINE.getService<NIKESAURUS::Assets::Service>()->loadTexture("PLAYER", "assets/Textures/player.png");
	//Load Font
	NIKE_ENGINE.getService<NIKESAURUS::Assets::Service>()->loadFont("MONTSERRAT", "assets/Fonts/Montserrat-Bold.ttf");

	//Load music
	NIKE_ENGINE.getService<NIKESAURUS::Assets::Service>()->loadMusic("MUSIC", "assets/Audios/test_music.wav");
	NIKE_ENGINE.getService<NIKESAURUS::Assets::Service>()->loadSound("SFX", "assets/Audios/test_sound.wav");
}

void Splash::Scene::init() {

	std::shared_ptr<NIKESAURUS::Scenes::Layer> base_Layer = registerLayer("BASE");
	std::shared_ptr<NIKESAURUS::Scenes::Layer> second_layer = registerLayer("SECOND");

	//Creat Channel Group
	NIKE_ENGINE.getService<NIKESAURUS::Audio::Service>()->createChannelGroup("MASTER");

	//Play Music
	NIKE_ENGINE.getService<NIKESAURUS::Audio::Service>()->playAudio("MUSIC", "MUSIC_CHANNEL", "MASTER", 1.0f, 1.0f, true);

	//Set Music Volume
	NIKE_ENGINE.getService<NIKESAURUS::Audio::Service>()->getChannelGroup("MASTER")->setVolume(0.5f);

	// Test Logger
	NIKEE_WARN("This is a warning message");

	//Create player
	NIKESAURUS::Entity::Type player_1 = NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->createEntity();
	second_layer->addEntity(player_1);
	NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Transform::Transform>(player_1, NIKESAURUS::Transform::Transform({0.0f, 0.0f}, {100.0f, 100.0f}, 0.0f));
	NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Transform::Velocity>(player_1, NIKESAURUS::Transform::Velocity());
	NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Render::Texture>(player_1, NIKESAURUS::Render::Texture("PLAYER", {0.0f, 0.0f, 0.0f, 1.0f}));
	NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Render::Cam>(player_1, NIKESAURUS::Render::Cam(NIKE_ENGINE.getService<NIKESAURUS::Windows::Service>()->getWindow()->getWindowSize().y));

	NIKE_ENGINE.getService<NIKESAURUS::Events::Service>()->dispatchEvent(std::make_shared<NIKESAURUS::Render::ChangeCamEvent>(player_1));

	NIKESAURUS::Entity::Type shape_1 = NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->createEntity();
	base_Layer->addEntity(shape_1);
	//NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->addDefEntityComponent(shape_1, NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->getAllComponentTypes().at("Render::Texture"));
	NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Transform::Transform>(shape_1, NIKESAURUS::Transform::Transform({0.0f, 0.0f}, {100.0f, 100.0f}, 0.0f));
	NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Render::Texture>(shape_1, NIKESAURUS::Render::Texture("PLAYER", {0.0f, 0.0f, 0.0f, 1.0f}));
	
	//Method 1
	 //NIKESAURUS::Entity::Type sfx_1 = NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->createEntity();
	 //NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Audio::SFX>(sfx_1, NIKESAURUS::Audio::SFX(true, "SFX", "MASTER", 0.5f, 1.0f));

	NIKESAURUS::Entity::Type sfx_1 = NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->createEntity();
	base_Layer->addEntity(sfx_1);
	NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->addDefEntityComponent(sfx_1, NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->getAllComponentTypes().at("Audio::SFX"));
	NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()->getEntityComponent<NIKESAURUS::Audio::SFX>(sfx_1) = { true, "SFX", "MASTER", 0.5f, 1.0f };
	// Test crash logger
	//LOG_CRASH("This is a test crash");
}

void Splash::Scene::exit() {

}

void Splash::Scene::unload() {

}
