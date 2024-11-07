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
	NIKE_ASSETS_SERVICE->reloadAssets("Textures");
	NIKE_ASSETS_SERVICE->reloadAssets("Shaders");
	NIKE_ASSETS_SERVICE->reloadAssets("Audio");
	NIKE_ASSETS_SERVICE->reloadAssets("Models");
	NIKE_ASSETS_SERVICE->reloadAssets("Fonts");
	NIKE_ASSETS_SERVICE->reloadAssets("Prefabs");
}

void Splash::Scene::init() {

	NIKE_SERIALIZE_SERVICE->loadSceneFromFile("assets/Scenes/test.scn");

	//NIKE_UI_SERVICE->createButton("Test",
	//	NIKE::Transform::Transform({ 0.0f,0.0f }, { 200.0f, 100.0f }, 0.0f),
	//	NIKE::Render::Text("Montserrat-Bold", "PLAY", { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f),
	//	NIKE::Render::Shape("square", { 1.0f, 0.0f, 0.0f, 1.0f }));

	//NIKE_UI_SERVICE->createButton("Test1",
	//	NIKE::Transform::Transform({ 0.0f,400.0f }, { 200.0f, 100.0f }, 0.0f),
	//	NIKE::Render::Text("Montserrat-Bold", "PLAY", { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f),
	//	NIKE::Render::Shape("square", { 1.0f, 0.0f, 0.0f, 1.0f }));

	//std::shared_ptr<NIKE::Scenes::Layer> base_Layer = createLayer();
	//std::shared_ptr<NIKE::Scenes::Layer> second_layer = createLayer();

	////base_Layer->setLayerMask(second_layer->getLayerID(), true);

	//Creat Channel Group
	NIKE_AUDIO_SERVICE->createChannelGroup("MASTER");

	//Play Music
	NIKE_AUDIO_SERVICE->playAudio("test_music", "MUSIC_CHANNEL", "MASTER", 1.0f, 1.0f, true);

	//Set Music Volume
	NIKE_AUDIO_SERVICE->getChannelGroup("MASTER")->setVolume(0.5f);

	//// Test Logger
	//NIKEE_WARN("This is a warning message");

	//NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Physics::ChangePhysicsEvent>(1.0f));

	////Create player
	//NIKE::Entity::Type player_1 = NIKE_ECS_SERVICE->createEntity(base_Layer->getLayerID());
	//NIKE_IMGUI_SERVICE->addEntityRef("player_1", player_1);
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(player_1, NIKE::Transform::Transform({0.0f, 200.0f}, {100.0f, 100.0f}, 0.0f));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Dynamics>(player_1, NIKE::Physics::Dynamics(200.0f, 1.0f, 2.0f));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Collider>(player_1, NIKE::Physics::Collider(NIKE::Physics::Resolution::BOUNCE));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(player_1, NIKE::Render::Texture("ZOMBIE", {1.0f, 0.0f, 0.0f, 1.0f}, true, 0.5f, false, {9, 5}, {0, 0}, {false, true}));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Cam>(player_1, NIKE::Render::Cam(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Animation::Base>(player_1, NIKE::Animation::Base(0, 0.2f));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Animation::Sprite>(player_1, NIKE::Animation::Sprite({9,5}, {0, 1}, {8, 1}));
	//NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::ChangeCamEvent>(player_1));

	//NIKE::Entity::Type shape_1 = NIKE_ECS_SERVICE->createEntity(second_layer->getLayerID());
	//NIKE_IMGUI_SERVICE->addEntityRef("shape_1", shape_1);
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Cam>(shape_1, NIKE::Render::Cam(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(shape_1, NIKE::Transform::Transform({0.0f, 0.0f}, {100.0f, 100.0f}, 0.0f));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Dynamics>(shape_1, NIKE::Physics::Dynamics(200.0f, 1.0f, 0.1f));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Collider>(shape_1, NIKE::Physics::Collider(NIKE::Physics::Resolution::NONE));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(shape_1, NIKE::Render::Texture("TREE", {1.0f, 1.0f, 1.0f, 1.0f}));

	//NIKE::Entity::Type shape_2 = NIKE_ECS_SERVICE->createEntity(base_Layer->getLayerID());
	//NIKE_IMGUI_SERVICE->addEntityRef("shape_2", shape_2);
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Cam>(shape_2, NIKE::Render::Cam(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(shape_2, NIKE::Transform::Transform({ 200.0f, 100.0f }, { 100.0f, 100.0f }, 0.0f));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(shape_2, NIKE::Render::Texture("TREE", { 1.0f, 1.0f, 1.0f, 1.0f }));
	//
	//NIKE::Entity::Type text_1 = NIKE_ECS_SERVICE->createEntity(base_Layer->getLayerID());
	//NIKE_IMGUI_SERVICE->addEntityRef("text_1", text_1);
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(text_1, NIKE::Transform::Transform({ 0.0f, -300.0f }, { 0.0f, 0.0f }, 45.0f));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Text>(text_1, NIKE::Render::Text("MONTSERRAT", "HELLO WORLD.", {1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, NIKE::Render::TextOrigin::CENTER));

	//NIKE::Entity::Type text_2 = NIKE_ECS_SERVICE->createEntity(base_Layer->getLayerID());
	//NIKE_IMGUI_SERVICE->addEntityRef("text_2", text_2);
	//NIKE_ECS_SERVICE->addEntityComponent(text_2, NIKE::Transform::Transform({ -750.0f, 400.0f }, { 0.0f, 0.0f }, 0.0f));
	//NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Text>(text_2, NIKE::Render::Text("MONTSERRAT", "PANTAT.", { 1.0f, 0.0f, 0.0f, 1.0f }, 1.0f, NIKE::Render::TextOrigin::LEFT));

	//NIKE::Entity::Type sfx_1 = NIKE_ECS_SERVICE->createEntity(base_Layer->getLayerID());
	//NIKE_ECS_SERVICE->addDefEntityComponent(sfx_1, NIKE_ECS_SERVICE->getAllComponentTypes().at("Audio::SFX"));
	//NIKE_ECS_SERVICE->getEntityComponent<NIKE::Audio::SFX>(sfx_1) = { true, "SFX", "MASTER", 0.5f, 1.0f };

	//NIKE_SERIALIZE_SERVICE->loadSceneFromFile("assets/Scenes/test.scn");
}

void Splash::Scene::exit() {
	//NIKE_SERIALIZE_SERVICE->saveSceneToFile("assets/Scenes/test.scn");
}

void Splash::Scene::unload() {

}
