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
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadShader("TEXTURE", "assets/Shaders/texture.vert", "assets/Shaders/texture.frag");

	//Load Models
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadModel("square", "assets/Models/square.txt");
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadModel("TRIANGLE", "assets/Models/triangle.txt");
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadModel("CIRCLE", "assets/Models/circle.txt");
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadModel("SQUARE_TEXTURE", "assets/Models/square-texture.txt");

	//Load Texture
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadTexture("PLAYER", "assets/Textures/player.png");

	//Load Font
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadFont("MONTSERRAT", "assets/Fonts/Montserrat-Bold.ttf");

	//Load music
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadMusic("MUSIC", "assets/Audios/test_music.wav");
	NIKEEngine.getService<NIKESAURUS::Assets::Service>()->loadSound("SFX", "assets/Audios/test_sound.wav");
}

void Splash::Scene::init() {

	//Creat Channel Group
	NIKEEngine.getService<NIKESAURUS::Audio::Service>()->createChannelGroup("MASTER");

	//Play Music
	NIKEEngine.getService<NIKESAURUS::Audio::Service>()->playAudio("MUSIC", "MUSIC_CHANNEL", "MASTER", 1.0f, 1.0f, true);

	//Set Music Volume
	NIKEEngine.getService<NIKESAURUS::Audio::Service>()->getChannelGroup("MASTER")->setVolume(0.5f);

	// Test Logger
	NIKEE_WARN("This is a warning message");

	//Create Shape
	NIKESAURUS::Entity::Type shape_1 = NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->createEntity();
	NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Transform::Transform>(shape_1, NIKESAURUS::Transform::Transform({0.0f, 0.0f}, {100.0f, 100.0f}, 0.0f));
	NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Render::Shape>(shape_1, NIKESAURUS::Render::Shape("TRIANGLE", {1.0f, 0.0f, 0.0f, 1.0f}));

	//Create entity
	// NIKESAURUS::Entity::Type sfx_1 = NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->createEntity();
	// NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addEntityComponent<NIKESAURUS::Audio::SFX>(sfx_1, NIKESAURUS::Audio::SFX(true, "SFX", "MASTER", 0.5f, 1.0f));
	
	// Test crash logger
	//LOG_CRASH("This is a test crash");
}

void Splash::Scene::exit() {

}

void Splash::Scene::unload() {

}
