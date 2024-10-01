/*****************************************************************//**
 * \file   Engine.h
 * \brief  
 * 
 * \author jings
 * \date   11 September 2024
 *********************************************************************/

#pragma once

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "../headers/Managers/mEntity.h"
#include "../headers/Managers/mComponent.h"
#include "../headers/Managers/mSystem.h"
#include "../headers/Managers/mEvents.h"
#include "../headers/Managers/mScene.h"
#include "../headers/Managers/mAssetManager.h"
#include "../headers/Managers/mCollision.h"

namespace Core {

	class Engine {
	private:

		//Default Constructor For Engine
		Engine();

		//Delete Copy Constructor & Copy Assignment
		Engine(Engine const& copy) = delete;
		void operator=(Engine const& copy) = delete;

		//Destructor
		~Engine();
		
		//Engine Variables
		GLFWwindow* ptr_window;
		Vector2 window_size;
		std::string window_title;

		//Delta time variables
		float delta_time;
		double curr_time;

		//FPS Variables
		int target_fps;
		float actual_fps;

		//Managers
		std::unique_ptr<Entity::Manager> entity_manager;
		std::unique_ptr<Component::Manager> component_manager;
		std::unique_ptr<System::Manager> system_manager;
		std::unique_ptr<Events::Manager> events_manager;
		std::unique_ptr<Asset::Manager> asset_manager;
		std::unique_ptr<Collision::Manager> collision_manager;

		//Scenes manager
		std::shared_ptr<Scenes::Manager> scenes_manager;

		/**
		 * Read & Deserialize Data From Config File
		 * Basic Data: Window Dimensions & Title
		 */
		void readConfigFile(std::string const& file_path);

		//Configure systems ( GLFW & GLEW )
		void configSystem();

		//Calculate Delta Time
		void calculateDeltaTime();

		//FPS control
		void controlFPS();

	public:

		/**
		 * instance manager for singleton class.
		 *
		 * \return instance
		 */
		static Engine& getInstance() {
			static Core::Engine instance;
			return instance;
		}


		// To get asset manager
		Asset::Manager& getAssetManager() {
			return *asset_manager;
		}

		//Init Window with config file
		void init(std::string const& file_path, int fps = 60);

		//Run Game Loop
		void run();

		//Terminate Game Loop
		void terminate();

		//Get Window
		GLFWwindow* getWindow() const;

		//Set Window Title
		void setWinTitle(std::string const& title);

		//Set Window Size
		void setWindowSize(float width, float height);

		//Get Window Size
		Vector2 const& getWindowSize() const;

		//Set Target FPS
		void setTargetFPS(int fps);

		//Get Current FPS
		float getCurrentFPS() const;

		//Get Delta Time
		float getDeltaTime() const;

		/*****************************************************************//**
		* Entity Methods
		*********************************************************************/
		//Create Entity
		Entity::Type createEntity();

		//Destroy Entity
		void destroyEntity(Entity::Type entity);

		/*****************************************************************//**
		* Component Methods
		*********************************************************************/
		template<typename T>
		void registerComponent() {
			component_manager->registerComponent<T>();
		}

		template<typename T>
		void addEntityComponentObj(Entity::Type entity, T&& component) {

			//Add component
			component_manager->addEntityComponentObj<T>(entity, std::move(component));

			//Set bit signature of component to true
			Component::Signature sign = entity_manager->getSignature(entity);
			sign.set(component_manager->getComponentType<T>(), true);
			entity_manager->setSignature(entity, sign);

			//Update entities list
			system_manager->updateEntitiesList(entity, sign, component_manager->getComponentType<T>(), true);
		}

		template<typename T>
		void removeEntityComponent(Entity::Type entity) {
			//Remove component
			component_manager->removeEntityComponent<T>(entity);

			//Set bit signature of component to false
			Component::Signature sign = entity_manager->getSignature(entity);
			sign.set(component_manager->getComponentType<T>(), false);
			entity_manager->setSignature(entity, sign);

			//Update entities list
			system_manager->updateEntitiesList(entity, sign, component_manager->getComponentType<T>(), false);
		}

		template<typename T>
		T& getEntityComponent(Entity::Type entity) {
			return component_manager->getEntityComponent<T>(entity);
		}

		template<typename T>
		bool checkEntityComponent(Entity::Type entity) {
			return component_manager->checkEntityComponent<T>(entity);
		}

		template<typename T>
		Component::Type getComponentType() {
			return component_manager->getComponentType<T>();
		}

		/*****************************************************************//**
		* System Methods
		*********************************************************************/
		template<typename T>
		std::shared_ptr<T> registerSystem(std::shared_ptr<T> singleton_sys = nullptr)
		{
			if (singleton_sys) {
				return system_manager->registerSystem<T>(singleton_sys);
			}
			else {
				return system_manager->registerSystem<T>();
			}
		}

		template<typename T>
		std::shared_ptr<T> accessSystem()
		{
			return system_manager->accessSystem<T>();
		}

		template<typename T>
		void addSystemComponentType(Component::Type component)
		{
			system_manager->addComponentType<T>(component);
		}

		/*****************************************************************//**
		* Scene Methods
		*********************************************************************/
		//Register scenes
		template<typename T>
		void registerScenes(std::string const& scene_id) {
			scenes_manager->registerScenes<T>(scene_id);
		}

		/*****************************************************************//**
		* Events Methods
		*********************************************************************/

		//Add event listener
		template<typename T>
		void addEventListeners(std::shared_ptr<Events::IEventListener> listener) {
			events_manager->addEventListeners<T>(listener);
		}

		//Dispatch event
		template<typename T>
		void dispatchEvent(std::shared_ptr<T> event) {
			events_manager->dispatchEvent<T>(event);
		}
	};

	//Predefined name for core engine
	#define NIKEEngine Core::Engine::getInstance()

	// Predefined name for asset manager
	#define ASSET_MANAGER NIKEEngine.getAssetManager()
}

#endif // !ENGINE_HPP