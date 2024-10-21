/*****************************************************************//**
 * \file   mScene.h
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef M_SCENE_HPP
#define M_SCENE_HPP

#include "Managers/ECS/mSystem.h"

namespace NIKESAURUS {
	namespace Scenes {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		//Scene manager actions
		enum class NIKESAURUS_API Actions {
			CHANGE = 0,
			RESTART,
			PREVIOUS,
			CLOSE
		};

		//Change Scene Event
		struct NIKESAURUS_API SceneEvent {
			Actions scene_action;
			std::string next_scene_id;

			//Constructor
			SceneEvent(Actions scene_action, std::string next_scene_id)
				: scene_action{ scene_action }, next_scene_id{ next_scene_id } {}
		};

		//Scene interface
		class NIKESAURUS_API IScene {
		public:
			//Default constructor
			IScene() = default;

			//Phases
			virtual void load() = 0;
			virtual void init() = 0;
			virtual void exit() = 0;
			virtual void unload() = 0;

			//Default virtual destructor
			virtual ~IScene() = default;
		};

		//Scenes manager
		class NIKESAURUS_API Manager {
		private:
			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

			//Map of scene files ( To be implemented )
			std::unordered_map<std::string, std::shared_ptr<IScene>> scenes;

			//Current scene
			std::shared_ptr<IScene> curr_scene;

			//Prev scene
			std::shared_ptr<IScene> prev_scene;

			//Scene event queue
			std::shared_ptr<Scenes::SceneEvent> event_queue;

			//Init starting scene
			void initScene(std::string scene_id);

			//Change scene
			void changeScene(std::string scene_id);

			//Restart scene
			void restartScene();

			//Go To Previous scene
			void previousScene();

		public:

			//Default Constructor
			Manager() = default;

			//Singleton Of Manager Class
			static std::shared_ptr<Manager> getInstance() {
				static std::shared_ptr<Manager> instance{ std::make_shared<Manager>() };
				return instance;
			}

			//Register scenes
			template<typename T>
			void registerScene(std::string const& scene_id) {

				//New scene
				std::shared_ptr<T> scene{ std::make_shared<T>() };

				//Construct scene obj
				scenes.emplace(std::piecewise_construct, std::forward_as_tuple(scene_id), std::forward_as_tuple(scene));

				//Default starting state will be the first state registered
				if (!curr_scene) {
					initScene(scene_id);
				}
			}

			//Get Current Scene
			std::string getCurrSceneID() const;

			//Queue scene event
			void queueSceneEvent(Scenes::SceneEvent&& event);

			//Update scene event
			void update();
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif // !M_SCENE_HPP