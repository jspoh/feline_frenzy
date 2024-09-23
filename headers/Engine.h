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

#include "Entity.h"
#include "Component.h"
#include "System.h"

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
		* Comonent Methods
		*********************************************************************/
		template<typename T>
		void registerComponent() {
			component_manager->registerComponent<T>();
		}

		template<typename T>
		void addEntityComponent(Entity::Type entity, T component) {

			//Add component
			component_manager->addEntityComponent<T>(entity, component);

			//Set bit signature of component to true
			Component::Signature sign = entity_manager->getSignature(entity);
			sign.set(component_manager->getComponentType<T>(), true);
			entity_manager->setSignature(entity, sign);

			//Update entities list
			system_manager->updateEntitiesList(entity, sign);
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
			system_manager->updateEntitiesList(entity, sign);
		}

		template<typename T>
		T& getEntityComponent(Entity::Type entity) {
			return component_manager->getEntityComponent<T>(entity);
		}

		template<typename T>
		Component::Type getComponentType() {
			return component_manager->getComponentType<T>();
		}

		/*****************************************************************//**
		* System Methods
		*********************************************************************/
		template<typename T>
		std::shared_ptr<T> addSystem()
		{
			return system_manager->addSystem<T>();
		}

		template<typename T>
		void setSystemSignature(Component::Signature signature)
		{
			system_manager->setSignature<T>(signature);
		}

		//Update system entities list
		void updateEntitiesList();
	};

	//Predefined name for core engine
	#define NIKEEngine Core::Engine::getInstance()
}

#endif // !ENGINE_HPP