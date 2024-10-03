/*****************************************************************//**
 * \file   SplashScene.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
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
#include "../headers/Systems/GameLogic/sysGameLogic.h"

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
}

void Splash::Scene::registerStaticSystems() {
	//Register input manager
	NIKEEngine.registerSystem<Input::Manager>(Input::Manager::getInstance());
	NIKEEngine.accessSystem<Input::Manager>()->setComponentsLinked(false);
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Key>());
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Mouse>());

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

	//Register font
	NIKEEngine.accessAssets()->registerFont("basic", "assets/Fonts/Montserrat-Bold.ttf");

	//Register audio
	NIKEEngine.accessAssets()->registerMusicAudio("assets/Audio/test_music.wav", "test_music");
	NIKEEngine.accessAssets()->createAudioGroup("test_group");

	//Register game logic components
	NIKEEngine.registerComponent<Scenes::ChangeSceneEvent>();
	NIKEEngine.registerComponent<GameLogic::ObjectSpawner>();

	//Register game logic system
	cout << NIKEEngine.getSystemIndex<Render::Manager>() << '\n';
	NIKEEngine.registerSystem<GameLogic::Manager>(nullptr, NIKEEngine.getSystemIndex<Render::Manager>());
	cout << NIKEEngine.getSystemIndex<Render::Manager>() << '\n';
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Scenes::ChangeSceneEvent>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<GameLogic::ObjectSpawner>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Input::Key>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Input::Mouse>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Move::Movement>());
	NIKEEngine.addSystemComponentType<GameLogic::Manager>(NIKEEngine.getComponentType<Transform::Velocity>());
	NIKEEngine.accessSystem<GameLogic::Manager>()->setComponentsLinked(false);
}

void Splash::Scene::init() {

	//Set BG Color
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	//Create music object
	bg_music = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Audio::cAudio>(bg_music, Audio::cAudio(NIKEEngine.accessAssets()->getAudio("test_music"), NIKEEngine.accessAssets()->getAudioGroup("test_group"), 1.f, 1.f, false));

	//Create new scene object
	next_scene = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Scenes::ChangeSceneEvent>(next_scene, { Scenes::Actions::CHANGE, "MENU" });
	NIKEEngine.addEntityComponentObj<Input::Mouse>(next_scene, { Input::TriggerMode::TRIGGERED });

	//Create entity with mouse & key component for testing
	input = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Key>(input, { Input::TriggerMode::TRIGGERED });
	NIKEEngine.addEntityComponentObj<Input::Mouse>(input, { Input::TriggerMode::TRIGGERED });
}

void Splash::Scene::exit() {
	NIKEEngine.destroyEntity(bg_music);
}

void Splash::Scene::unload() {

}
