/*****************************************************************//**
 * \file   SplashScene.cpp
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/SplashScene.h"

//Registered Audios
#include "../headers/Components/cInput.h"
#include "../headers/Components/cRender.h"
#include "../headers/Components/cAudio.h"
#include "../headers/Components/cMove.h"

//Registered Systems
#include "../headers/Systems/sysInput.h"
#include "../headers/Systems/Physics/sysPhysics.h"
#include "../headers/Systems/Animation/sysAnimation.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Systems/sysAudio.h"
#include "../headers/Systems/GameLogic/sysAnimationController.h"

void Splash::Scene::registerComponents() {
	//Register input components
	NIKEEngine.registerComponent<Input::Key>();
	NIKEEngine.registerComponent<Input::Mouse>();

	//Register physics components
	NIKEEngine.registerComponent<Transform::Velocity>();
	NIKEEngine.registerComponent<Transform::Transform>();
	NIKEEngine.registerComponent<Transform::Runtime_Transform>();
	NIKEEngine.registerComponent<Move::Movement>();
	NIKEEngine.registerComponent<Collision::Collider>();

	//Register animation components
	NIKEEngine.registerComponent<Animation::cBase>();
	NIKEEngine.registerComponent<Animation::cSprite>();

	//Register render components
	NIKEEngine.registerComponent<Render::Shape>();
	NIKEEngine.registerComponent<Render::Texture>();
	NIKEEngine.registerComponent<Render::Color>();
	NIKEEngine.registerComponent<Render::Cam>();
	NIKEEngine.registerComponent<Render::Text>();

	//Register audio components
	NIKEEngine.registerComponent<Audio::cAudio>();

	//Register scene components
	NIKEEngine.registerComponent<Scenes::ChangeScene>();
}

void Splash::Scene::registerStaticSystems() {
	//Register input manager
	NIKEEngine.registerSystem<Input::Manager>(Input::Manager::getInstance());
	NIKEEngine.accessSystem<Input::Manager>()->setComponentsLinked(false);
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Key>());
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Mouse>());
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Transform::Runtime_Transform>());

	//Register physics manager
	NIKEEngine.registerSystem<Physics::Manager>(Physics::Manager::getInstance());
	NIKEEngine.accessSystem<Physics::Manager>()->setComponentsLinked(false);
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Transform::Velocity>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Transform::Runtime_Transform>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Transform::Transform>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType<Move::Movement>());
	NIKEEngine.addSystemComponentType<Physics::Manager>(NIKEEngine.getComponentType <Collision::Collider>());

	//Register animation manager
	NIKEEngine.registerSystem<Animation::Manager>(Animation::Manager::getInstance());
	NIKEEngine.accessEvents()->addEventListeners<Animation::AnimationEvent>(NIKEEngine.accessSystem<Animation::Manager>());
	NIKEEngine.addSystemComponentType<Animation::Manager>(NIKEEngine.getComponentType<Animation::cBase>());
	NIKEEngine.addSystemComponentType<Animation::Manager>(NIKEEngine.getComponentType<Animation::cSprite>());
	NIKEEngine.addSystemComponentType<Animation::Manager>(NIKEEngine.getComponentType<Render::Texture>());

	//Register render manager
	NIKEEngine.registerSystem<Render::Manager>(Render::Manager::getInstance());
	NIKEEngine.accessSystem<Render::Manager>()->setComponentsLinked(false);
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Transform::Transform>());
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Render::Shape>());
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Render::Texture>());
	NIKEEngine.addSystemComponentType<Render::Manager>(NIKEEngine.getComponentType<Render::Text>());

	//Register audio manager
	NIKEEngine.registerSystem<Audio::Manager>(Audio::Manager::getInstance());
	NIKEEngine.addSystemComponentType<Audio::Manager>(NIKEEngine.getComponentType<Audio::cAudio>());

	//Add components for scene manager
	NIKEEngine.addSystemComponentType<Scenes::Manager>(NIKEEngine.getComponentType<Scenes::ChangeScene>());
	NIKEEngine.addSystemComponentType<Scenes::Manager>(NIKEEngine.getComponentType<Input::Mouse>());
	NIKEEngine.addSystemComponentType<Scenes::Manager>(NIKEEngine.getComponentType<Input::Key>());
	NIKEEngine.accessSystem<Scenes::Manager>()->setComponentsLinked(false);
}

void Splash::Scene::load() {

	//Register components
	registerComponents();

	//Register static systems
	registerStaticSystems();

	//Register shader
	NIKEEngine.accessAssets()->registerShader("base", "shaders/base.vert", "shaders/base.frag");
	NIKEEngine.accessAssets()->registerShader("texture", "shaders/texture.vert", "shaders/texture.frag");

	//Register models
	NIKEEngine.accessAssets()->registerModel("square", "assets/meshes/square.txt");
	NIKEEngine.accessAssets()->registerModel("triangle", "assets/meshes/triangle.txt");
	NIKEEngine.accessAssets()->registerModel("circle", "assets/meshes/circle.txt");
	NIKEEngine.accessAssets()->registerModel("square-texture", "assets/meshes/square-texture.txt");

	//Register texture
	NIKEEngine.accessAssets()->registerTexture("zombie", "assets/textures/ZombieSheet.png");
	NIKEEngine.accessAssets()->registerTexture("bg", "assets/textures/background.jpg");

	//Register font
	NIKEEngine.accessAssets()->registerFont("basic", "assets/Fonts/Montserrat-Bold.ttf");

	//Register audio
	NIKEEngine.accessAssets()->registerMusicAudio("assets/Audio/test_music.wav", "test_music");
	NIKEEngine.accessAssets()->createAudioGroup("test_group");

	//Register animation event components
	NIKEEngine.registerComponent<Animation::SpriteEvent>();
	NIKEEngine.registerComponent<Animation::AnimationEvent>();

	NIKEEngine.registerSystem<AnimationController::Manager>(nullptr, NIKEEngine.getSystemIndex<Animation::Manager>());
	NIKEEngine.addSystemComponentType<AnimationController::Manager>(NIKEEngine.getComponentType<Animation::SpriteEvent>());
}

void Splash::Scene::init() {

	//Set BG Color
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	//Create music object
	entities["bg_music"] = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Audio::cAudio>(entities["bg_music"], Audio::cAudio(NIKEEngine.accessAssets()->getAudio("test_music"), NIKEEngine.accessAssets()->getAudioGroup("test_group"), 1.f, 1.f, false));

	//Create new scene object
	entities["next_scene"] = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Scenes::ChangeScene>(entities["next_scene"], { Scenes::Actions::CHANGE, "MENU", -1, GLFW_KEY_RIGHT });
	NIKEEngine.addEntityComponentObj<Input::Key>(entities["next_scene"], { Input::TriggerMode::TRIGGERED });

	//Create animation object
	entities["background"] = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities["background"], { {0.0f, -50.0f}, {1920.0f, 1080.0f}, 0.0f });
	NIKEEngine.addEntityComponentObj<Render::Texture>(entities["background"], { "bg", Matrix33::Matrix_33::Identity(), {{0.0f, 0.0f, 0.0f}, 0.75f}, {1920.0f, 1080.0f}, {1.0f , 1.0f}, {0.0f,0.0f} });

	//Create animation object
	entities["animation"] = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Transform::Transform>(entities["animation"], { {0.0f, 0.0f}, { 1280.0f / 6, 1728.0f / 6}, 0.0f });
	NIKEEngine.addEntityComponentObj<Render::Texture>(entities["animation"], { "zombie", Matrix33::Matrix_33::Identity(), {{0.0f, 0.0f, 0.0f}, 1.0f}, {1728.0f, 1280.0f}, {1.0f / 9.0f, 1.0f / 5.0f}, {0.0f,0.0f} });
	NIKEEngine.addEntityComponentObj<Animation::cBase>(entities["animation"], Animation::cBase("animate_zombie", 0, 1.0f));
	NIKEEngine.addEntityComponentObj<Animation::cSprite>(entities["animation"], Animation::cSprite(Vector2(9.0f, 5.0f), Vector2(0.0f, 4.0f), Vector2(7.0f, 4.0f)));
	NIKEEngine.addEntityComponentObj<Animation::SpriteEvent>(entities["animation"], { {0.0f, 0.0f}, {7.0f, 0.0f}, "animate_zombie", 1, -1, GLFW_KEY_SPACE});
	NIKEEngine.addEntityComponentObj<Input::Key>(entities["animation"], { Input::TriggerMode::TRIGGERED });

	//Create text object
	Entity::Type basic_text = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Render::Text>(basic_text, { "basic", "PRESS SPACE TO JUMP", {{0.0f, 0.0f, 0.0f}, 1.0f}, {-0.5f, 0.5f}, 0.05f });
}

void Splash::Scene::exit() {
	NIKEEngine.destroyEntity(entities["next_scene"]);
	NIKEEngine.destroyEntity(entities["animation"]);
}

void Splash::Scene::unload() {

}
