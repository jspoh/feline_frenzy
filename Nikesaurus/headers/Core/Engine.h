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

#include "../headers/Managers/mWindows.h"
#include "../headers/Managers/mScenes.h"
#include "../headers/Managers/mEvents.h"
#include "../headers/Managers/mAssets.h"
#include "../headers/Managers/mDebug.h"
#include "../headers/Managers/mSerialization.h"

#include "../headers/Managers/ECS/mCoordinator.h"

namespace NIKESAURUS {
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

			//Standalone Managers
			std::unique_ptr<Windows::Manager> windows_manager;
			std::unique_ptr<Scenes::Manager> scenes_manager;
			std::unique_ptr<Events::Manager> events_manager;
			std::unique_ptr<Assets::Manager> assets_manager;
			std::unique_ptr<Debug::Manager> debug_manager;
			std::unique_ptr<Serialization::Manager> seri_manager;

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
			std::unique_ptr<Windows::Manager>& accessWindow();

			/*****************************************************************//**
			* Access Scene Functions
			*********************************************************************/
			std::unique_ptr<Scenes::Manager>& accessScenes();

			/*****************************************************************//**
			* Access Events
			*********************************************************************/
			std::unique_ptr<Events::Manager>& accessEvents();

			/*****************************************************************//**
			* Assets Methods
			*********************************************************************/
			std::unique_ptr<Assets::Manager>& accessAssets();

			/*****************************************************************//**
			* Debug Methods
			*********************************************************************/
			std::unique_ptr<Debug::Manager>& accessDebug();

			/*****************************************************************//**
			* Serialization Methods
			*********************************************************************/
			std::unique_ptr<Serialization::Manager>& accessSeri();
		};

		//Predefined name for core engine
#define NIKEEngine Core::Engine::getInstance()
	}
}

#endif // !ENGINE_HPP