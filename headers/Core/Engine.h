/*****************************************************************//**
 * \file   Engine.h
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   11 September 2024
 *********************************************************************/

#pragma once

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "../headers/Managers/mEntity.h"
#include "../headers/Managers/mComponent.h"
#include "../headers/Managers/mSystem.h"
#include "../headers/Managers/mWindows.h"
#include "../headers/Managers/mEvents.h"
#include "../headers/Managers/mCollision.h"
#include "../headers/Managers/mAssets.h"
#include "../headers/Managers/mDebug.h"
#include "../headers/Managers/mSerialization.h"

namespace Core {

	class Engine {
	private:

		//Default Constructor For Engine
		Engine() = default;

		//Delete Copy Constructor & Copy Assignment
		Engine(Engine const& copy) = delete;
		void operator=(Engine const& copy) = delete;

		//Destructor
		~Engine();

		//Coordinated Managers
		std::unique_ptr<Entity::Manager> entity_manager;
		std::unique_ptr<Component::Manager> component_manager;
		std::unique_ptr<System::Manager> system_manager;

		//Standalone Managers
		std::unique_ptr<Windows::Manager> windows_manager;
		std::unique_ptr<Events::Manager> events_manager;
		std::unique_ptr<Assets::Manager> assets_manager;
		std::unique_ptr<Debug::Manager> debug_manager;
		std::unique_ptr<Serialization::Manager> seri_manager;

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

		/*****************************************************************//**
		* Entity Methods
		*********************************************************************/
		//Create Entity
		Entity::Type createEntity();

		//Clone entity ( ID of clone returned )
		Entity::Type cloneEntity(Entity::Type copy);

		//Destroy Entity
		void destroyEntity(Entity::Type entity);

		//Destroy Entity
		void destroyAllEntities();

		//Get entity count
		int getEntitiesCount();

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
		std::shared_ptr<T> registerSystem(std::shared_ptr<T> singleton_sys = nullptr, int index = -1)
		{
			return system_manager->registerSystem<T>(singleton_sys, index);
		}

		template<typename T>
		void removeSystem(std::shared_ptr<T> singleton_sys = nullptr)
		{
			system_manager->removeSystem<T>();
		}

		template<typename T>
		std::shared_ptr<T> accessSystem()
		{
			return system_manager->accessSystem<T>();
		}

		template<typename T>
		int getSystemIndex()
		{
			return system_manager->getSystemIndex<T>();
		}

		template<typename T>
		void addSystemComponentType(Component::Type component)
		{
			system_manager->addComponentType<T>(component);
		}

		/*****************************************************************//**
		* Access Window Functions
		*********************************************************************/
		std::unique_ptr<Windows::Manager>& accessWindow();

		/*****************************************************************//**
		* Access Events
		*********************************************************************/
		std::unique_ptr<Events::Manager>& accessEvents();

		/*****************************************************************//**
		* Assets Methods
		*********************************************************************/
		std::unique_ptr<Assets::Manager>& accessAssets();
		std::unique_ptr<Debug::Manager>& accessDebug();
		std::unique_ptr<Serialization::Manager>& accessSeri();
	};

	//Predefined name for core engine
	#define NIKEEngine Core::Engine::getInstance()
}

#endif // !ENGINE_HPP