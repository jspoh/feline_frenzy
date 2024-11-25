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

#include "Core/Engine.h"

void Splash::Scene::load() {
	NIKE_ASSETS_SERVICE->reloadAssets("Textures");
	NIKE_ASSETS_SERVICE->reloadAssets("Shaders");
	NIKE_ASSETS_SERVICE->reloadAssets("Audio");
	NIKE_ASSETS_SERVICE->reloadAssets("Models");
	NIKE_ASSETS_SERVICE->reloadAssets("Fonts");
	NIKE_ASSETS_SERVICE->reloadAssets("Scripts");
}

void Splash::Scene::init() {

	NIKE_SERIALIZE_SERVICE->loadSceneFromFile("assets/Scenes/audio_scene.scn");

	//NIKE_UI_SERVICE->createButton("Test",
	//	NIKE::Transform::Transform({ 0.0f,0.0f }, { 200.0f, 100.0f }, 0.0f),
	//	NIKE::Render::Text("Montserrat-Bold", "PLAY", { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f),
	//	NIKE::Render::Shape("square", { 1.0f, 0.0f, 0.0f, 1.0f }));

	//NIKE_UI_SERVICE->createButton("Button",
	//	NIKE::Transform::Transform({ 0.0f, 0.0f }, { 200.0f, 100.0f }, 0.0f),
	//	NIKE::Render::Text("Montserrat-Bold", "Button", { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f),
	//	NIKE::Render::Shape("square", { 1.0f, 0.0f, 0.0f, 1.0f }));

	//Creat Channel Group
	NIKE_AUDIO_SERVICE->createChannelGroup("MASTER");
	NIKE_AUDIO_SERVICE->createChannelGroup("Music");
	NIKE_AUDIO_SERVICE->createChannelGroup("SFX");
	////Play Music
	//NIKE_AUDIO_SERVICE->playAudio("test_music", "MUSIC_CHANNEL", "MASTER", 1.0f, 1.0f, true);

	//Set Music Volume
	//NIKE_AUDIO_SERVICE->getChannelGroup("MASTER")->setVolume(0.5f);

}

void Splash::Scene::exit() {
	//NIKE_SERIALIZE_SERVICE->saveSceneToFile("assets/Scenes/collision_scene.scn");
}

void Splash::Scene::unload() {

}