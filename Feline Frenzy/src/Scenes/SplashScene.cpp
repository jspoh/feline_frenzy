#include "../headers/Core/pch.h"

///*****************************************************************//**
// * \file   SplashScene.cpp
// * \brief
// * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (90%)
// * \co-author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (10%)
// * \date   September 2024
// * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
// *********************************************************************/
//
//#include "../headers/Core/stdafx.h"
//#include "../headers/Scenes/SplashScene.h"
//
////Components
//#include "../headers/Components/cAnimation.h"
//#include "../headers/Components/cTransform.h"
//#include "../headers/Components/cInput.h"
//#include "../headers/Components/cAudio.h"
//
////Systems
//#include "../headers/Systems/GameLogic/sysAnimationController.h"
//#include "../headers/Systems/Animation/sysAnimation.h"
//
//
//void Splash::Scene::load() {
//
//	//Register texture
//	NIKEEngine.accessAssets()->registerTexture("zombie", "assets/textures/ZombieSheet.png");
//	NIKEEngine.accessAssets()->registerTexture("bg", "assets/textures/background.jpg");
//
//	//Register audio
//	NIKEEngine.accessAssets()->registerMusicAudio("assets/Audio/test_music.wav", "test_music");
//	NIKEEngine.accessAssets()->createAudioGroup("test_group");
//
//	//Register font
//	NIKEEngine.accessAssets()->registerFont("basic", "assets/Fonts/Montserrat-Bold.ttf");
//
//	//Register animation event components
//	NIKEEngine.registerComponent<Animation::SpriteEvent>();
//	//NIKEEngine.registerComponent<Animation::AnimationEvent>();
//
//	NIKEEngine.registerSystem<AnimationController::Manager>(nullptr, NIKEEngine.getSystemIndex<Animation::Manager>());
//	NIKEEngine.addSystemComponentType<AnimationController::Manager>(NIKEEngine.getComponentType<Animation::SpriteEvent>());
//}
//
//void Splash::Scene::init() {
//
//	//Set BG Color
//	glClearColor(0, 0, 0, 0);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	//Create music object
//	entities["bg_music"] = NIKEEngine.createEntity();
//	NIKEEngine.addEntityComponentObj<Audio::cAudio>(entities["bg_music"], Audio::cAudio(NIKEEngine.accessAssets()->getAudio("test_music"), NIKEEngine.accessAssets()->getAudioGroup("test_group"), 1.f, 1.f, false));
//
//	//Create animation object
//	entities["background"] = NIKEEngine.createEntity();
//	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities["background"], { {0.0f, -50.0f}, {1920.0f, 1080.0f}, 0.0f });
//	NIKEEngine.addEntityComponentObj<Render::Texture>(entities["background"], { "bg", Matrix33::Matrix_33::Identity(), {{0.0f, 0.0f, 0.0f}, 0.75f}, {1920.0f, 1080.0f}, {1.0f , 1.0f}, {0.0f,0.0f} });
//
//	//Create animation object
//	entities["animation"] = NIKEEngine.createEntity();
//	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities["animation"], { {0.0f, 0.0f}, { 1280.0f / 6, 1728.0f / 6}, 0.0f });
//	NIKEEngine.addEntityComponentObj<Render::Texture>(entities["animation"], { "zombie", Matrix33::Matrix_33::Identity(), {{0.0f, 0.0f, 0.0f}, 1.0f}, {1728.0f, 1280.0f}, {1.0f / 9.0f, 1.0f / 5.0f}, {0.0f,0.0f} });
//	NIKEEngine.addEntityComponentObj<Animation::cBase>(entities["animation"], Animation::cBase("animate_zombie", 0, 1.0f));
//	NIKEEngine.addEntityComponentObj<Animation::cSprite>(entities["animation"], Animation::cSprite(Vector2(9.0f, 5.0f), Vector2(0.0f, 4.0f), Vector2(7.0f, 4.0f)));
//	NIKEEngine.addEntityComponentObj<Animation::SpriteEvent>(entities["animation"], { {0.0f, 0.0f}, {7.0f, 0.0f}, "animate_zombie", 1, -1, GLFW_KEY_SPACE});
//	NIKEEngine.addEntityComponentObj<Input::Key>(entities["animation"], { Input::TriggerMode::TRIGGERED });
//
//	//Create text object
//	Entity::Type basic_text = NIKEEngine.createEntity();
//	NIKEEngine.addEntityComponentObj<Render::Text>(basic_text, { "basic", "PRESS SPACE TO JUMP", {{0.0f, 0.0f, 0.0f}, 1.0f}, {-0.5f, 0.5f}, 0.05f });
//}
//
//void Splash::Scene::exit() {
//
//	//!TODO: IMPLEMENT UNLOADING OF ASSETS
//
//	//Cleanup
//	NIKEEngine.destroyAllEntities();
//
//	//Remove component and systems that are not in use
//	NIKEEngine.removeComponent<Animation::SpriteEvent>();
//	NIKEEngine.removeSystem<AnimationController::Manager>();
//}
//
//void Splash::Scene::unload() {
//
//}
