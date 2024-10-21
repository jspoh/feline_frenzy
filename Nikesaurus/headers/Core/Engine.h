/*****************************************************************//**
 * \file   Engine.h
 * \brief  Core engine arhcitecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef ENGINE_HPP
#define ENGINE_HPP

//Services
#include "Managers/mWindows.h"
#include "Managers/mScenes.h"
#include "Managers/mEvents.h"
//#include "Managers/mAssets.h"
#include "Managers/mDebug.h"
#include "Managers/mSerialization.h"
#include "Managers/ECS/mCoordinator.h"

namespace NIKESAURUS {
	namespace Core {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		class NIKESAURUS_API Engine {
		private:

			//Default Constructor For Engine
			Engine() = default;

			//Delete Copy Constructor & Copy Assignment
			Engine(Engine const& copy) = delete;
			void operator=(Engine const& copy) = delete;

			//Destructor
			~Engine();

			//Map of services
			static std::unordered_map<std::string, std::shared_ptr<void>> services;

			//Dependency Managers
			std::shared_ptr<Windows::Manager> windows_manager;
			std::shared_ptr<Scenes::Manager> scenes_manager;
			std::shared_ptr<Coordinator::Manager> ecs_coordinator;

			//Register default components
			void registerDefComponents();

			//Register default assets
			void registerDefSystems();

			//Register default assets
			void registerDefAssets();

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
			* Access Window Functions
			*********************************************************************/
			template<typename T>
			static void provideService(std::shared_ptr<T> service) {

				//Get service name for mapping
				std::string service_name{ typeid(T).name() };

				//Check if service has been provided before
				if (services.find(service_name) != services.end()) {
					throw std::runtime_error("Service already provided.");
				}

				//Insert into services map
				services.insert({ service_name, std::move(service) });
			}

			template<typename T>
			static std::shared_ptr<T> getService() {

				//Get service name for searching
				std::string service_name{ typeid(T).name() };

				//Check if service exists
				if (services.find(service_name) == services.end()) {
					throw std::runtime_error("Service doesnt exist.");
				}

				//Return service
				return std::static_pointer_cast<T>(services.at(service_name));
			}
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

//Predefined variable for the core engine
static NIKESAURUS::Core::Engine& NIKEEngine = NIKESAURUS::Core::Engine::getInstance();

#endif // !ENGINE_HPP
