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
#include "Managers/Services/sWindows.h"
#include "Managers/Services/sScenes.h"
#include "Managers/Services/sEvents.h"
#include "Managers/Services/sInput.h"
#include "Managers/Services/sDebug.h"
#include "Managers/Services/sSerialization.h"
#include "Managers/Services/sAudio.h"
#include "Managers/Services/Assets/sAssets.h"
#include "Managers/Services/Imgui/sImgui.h"
#include "Managers/Services/sCoordinator.h"

//ECS manager for internal usage
#ifdef NIKE_BUILD_DLL
#include "Managers/ECS/mCoordinator.h"
#endif

namespace NIKESAURUS {
	namespace Core {

		//Disable DLL Export warning
		#pragma warning(disable: 4251)

		class NIKESAURUS_API Engine {
		private:

			//Default Constructor For Engine
			Engine() = default;

			//Delete Copy Constructor & Copy Assignment
			Engine(Engine const& copy) = delete;
			void operator=(Engine const& copy) = delete;

			//Destructor
			~Engine() = default;

			//Map of services
			static std::unordered_map<std::string, std::shared_ptr<void>> services;

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
			void init(std::string const& file_path, int fps = 60, std::string const& custom_welcome = "Welcome.");

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
				return std::static_pointer_cast<T>(services.find(service_name)->second);
			}
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

//Core Engine Macros
#define NIKE_ENGINE NIKESAURUS::Core::Engine::getInstance()
#define NIKE_PROVIDE_SERVICE(service) NIKESAURUS::Core::Engine::getInstance().provideService<service>(std::make_shared<service>())
#define NIKE_GET_SERVICE(service) NIKESAURUS::Core::Engine::getInstance().getService<service>()
#define NIKE_WINDOWS_SERVICE NIKESAURUS::Core::Engine::getInstance().getService<NIKESAURUS::Windows::Service>()
#define NIKE_SCENES_SERVICE NIKESAURUS::Core::Engine::getInstance().getService<NIKESAURUS::Scenes::Service>()
#define NIKE_EVENTS_SERVICE NIKESAURUS::Core::Engine::getInstance().getService<NIKESAURUS::Events::Service>()
#define NIKE_INPUT_SERVICE NIKESAURUS::Core::Engine::getInstance().getService<NIKESAURUS::Input::Service>()
#define NIKE_AUDIO_SERVICE NIKESAURUS::Core::Engine::getInstance().getService<NIKESAURUS::Audio::Service>()
#define NIKE_ASSETS_SERVICE NIKESAURUS::Core::Engine::getInstance().getService<NIKESAURUS::Assets::Service>()
#define NIKE_DEBUG_SERVICE NIKESAURUS::Core::Engine::getInstance().getService<NIKESAURUS::Debug::Service>()
#define NIKE_IMGUI_SERVICE NIKESAURUS::Core::Engine::getInstance().getService<NIKESAURUS::IMGUI::Service>()
#define NIKE_ECS_SERVICE NIKESAURUS::Core::Engine::getInstance().getService<NIKESAURUS::Coordinator::Service>()

//ECS manager for internal usage
#ifdef NIKE_BUILD_DLL
#define NIKE_ECS_MANAGER NIKESAURUS::Core::Engine::getInstance().getService<NIKESAURUS::Coordinator::Manager>()
#endif

#endif // !ENGINE_HPP
