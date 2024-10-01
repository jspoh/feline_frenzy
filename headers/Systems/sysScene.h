/*****************************************************************//**
 * \file   mScene.h
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef M_STATE_HPP
#define M_STATE_HPP

#include "../headers/Managers/mEvents.h"
#include "../headers/Managers/mSystem.h"

namespace Scenes {

	//Scene interface
	class IScene {
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

	//Scene manager actions
	enum class Actions {
		IDLE = 0,
		CHANGE,
		RESTART,
		PREVIOUS
	};

	//Change Scene Event
	class ChangeSceneEvent : public Events::IEvent {
	public:
		Actions scene_action;
		std::string next_scene_id;

		//Default constructor
		ChangeSceneEvent() : scene_action { Actions::IDLE }, next_scene_id{""} {}

		//Constructor
		ChangeSceneEvent(Actions action, std::string next_scene)
			: scene_action{ action }, next_scene_id{ next_scene }
		{}
	};

	//Scenes manager
	class Manager : public System::ISystem, public Events::IEventListener {
	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		//Map of scene files ( To be implemented )
		std::unordered_map<std::string, std::shared_ptr<IScene>> states;

		//Current scene
		std::shared_ptr<IScene> curr_scene;

		//Prev scene
		std::shared_ptr<IScene> prev_scene;

		//Change scene event queue
		ChangeSceneEvent new_scene;

		//Init starting scene
		void initScene(std::string const& scene_id);

		//Change scene
		void changeScene(std::string const& scene_id);

		//Restart scene
		void restartScene();

		//Go To Previous scene
		void previousScene();

		//Register scenes
		template<typename T>
		void registerScenes(std::string const& scene_id) {

			//New scene
			std::shared_ptr<T> scene{ std::make_shared<T>() };

			//Construct scene obj
			states.emplace(std::piecewise_construct, std::forward_as_tuple(scene_id), std::forward_as_tuple(scene));

			//Default starting state will be the first state registered
			if (!curr_scene) {
				initScene(scene_id);
			}
		}

	public:

		//Default Constructor
		Manager() = default;

		//Singleton Of Manager Class
		static std::shared_ptr<Manager> getInstance() {
			static std::shared_ptr<Manager> instance{ std::make_shared<Manager>() };
			return instance;
		}

		//Init
		void init() override;

		//Update
		void update() override;

		//Execute event
		void executeEvent(std::shared_ptr<Events::IEvent> event) override;
	};
}

#endif // !M_STATE_HPP