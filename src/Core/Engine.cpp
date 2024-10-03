/*****************************************************************//**
 * \file   Engine.cpp
 * \brief  Graphics library manager for engine
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Core/Engine.h"

Core::Engine::~Engine() {
	glfwTerminate();
}

void Core::Engine::init(std::string const& file_path, int fps) {
	entity_manager = std::make_unique<Entity::Manager>();
	component_manager = std::make_unique<Component::Manager>();
	system_manager = std::make_unique<System::Manager>();
	windows_manager = std::make_unique<Windows::Manager>();
	events_manager = std::make_unique<Events::Manager>();
	assets_manager = std::make_unique<Assets::Manager>();
	debug_manager = std::make_unique<Debug::Manager>();
	seri_manager = std::make_unique<Serialization::Manager>();

	//Read config file
	windows_manager->readConfigFile(file_path);

	//Config glfw window system
	windows_manager->configSystem();

	//Set Target FPS
	windows_manager->setTargetFPS(fps);
}

void Core::Engine::run() {

	while (!glfwWindowShouldClose(windows_manager->getWindow())) {

		//Calculate Delta Time
		windows_manager->calculateDeltaTime();

		//Poll system events ( Interativity with app )
		glfwPollEvents();

		//Set Window Title
		windows_manager->setWinTitle(windows_manager->getWinTitle() + " | " + std::to_string(windows_manager->getCurrentFPS()) + " fps");

		//Update all systems
		system_manager->updateSystems();

		//Might move this into render system
		glfwSwapBuffers(windows_manager->getWindow());

		//Check if window is open
		if (glfwWindowShouldClose(windows_manager->getWindow())) {

			//Terminate window
			windows_manager->terminate();
		}

		//Control FPS
		windows_manager->controlFPS();
	}
}

/*****************************************************************//**
* Entity Methods
*********************************************************************/
Entity::Type Core::Engine::createEntity() {
	return entity_manager->createEntity();
}

Entity::Type Core::Engine::cloneEntity(Entity::Type copy) {
	Entity::Type new_entity = entity_manager->createEntity();
	component_manager->cloneEntity(new_entity, copy);
	entity_manager->setSignature(new_entity, entity_manager->getSignature(copy));
	system_manager->cloneEntity(new_entity, copy);

	return new_entity;
}

void Core::Engine::destroyEntity(Entity::Type entity) {

	//Destroy all data related to entity
	entity_manager->destroyEntity(entity);
	component_manager->entityDestroyed(entity);
	system_manager->entityDestroyed(entity);
}

/*****************************************************************//**
* Access Windows
*********************************************************************/
std::unique_ptr<Windows::Manager>& Core::Engine::accessWindow() {
	return windows_manager;
}

/*****************************************************************//**
* Access Events
*********************************************************************/
std::unique_ptr<Events::Manager>& Core::Engine::accessEvents() {
	return events_manager;
}

/*****************************************************************//**
* Access Assets
*********************************************************************/
std::unique_ptr<Assets::Manager>& Core::Engine::accessAssets() {
	return assets_manager;
}

/*****************************************************************//**
* Access Debug
*********************************************************************/

std::unique_ptr<Debug::Manager>& Core::Engine::accessDebug()
{
	return debug_manager;
}

std::unique_ptr<Serialization::Manager>& Core::Engine::accessSeri()
{
	return seri_manager;
}
