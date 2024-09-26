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

namespace Scenes {

	//Scene interface
	class IScene {
	public:
		//Default constructor
		IScene() = default;

		//Phases
		virtual void load() = 0;
		virtual void init() = 0;

		virtual void render() = 0;

		virtual void exit() = 0;
		virtual void unload() = 0;

		//Default virtual destructor
		virtual ~IScene() = default;
	};

	//State manager
	class Manager {
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

	public:

		//Default Constructor
		Manager() = default;

		//Register scenes
		template<typename T>
		void registerScenes(std::string const& scene_id) {

			//New scene
			std::shared_ptr<T> new_scene{ std::make_shared<T>() };

			//Construct scene obj
			states.emplace(std::piecewise_construct, std::forward_as_tuple(scene_id), std::forward_as_tuple(new_scene));

			//Default starting state will be the first state registered
			if (!curr_scene) {
				initScene(scene_id);
			}
		}

		//Init starting scene
		void initScene(std::string const& scene_id);

		//Change scene
		void changeScene(std::string const& scene_id);

		//Restart scene
		void restartScene();

		//Go To Previous scene
		void previousScene();

		//Update ( to be removed )
		void render();
	};
}

#endif // !M_STATE_HPP