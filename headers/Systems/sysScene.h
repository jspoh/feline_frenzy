/*****************************************************************//**
 * \file   mScene.h
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef M_STATE_HPP
#define M_STATE_HPP

#include "../headers/Managers/mSystem.h"
#include "../headers/Components/cScene.h"

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

	//Scenes manager
	class Manager : public System::ISystem {
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

		//Init
		void init() override;

		std::string getSysName() override
		{
			return "Scene System";
		}

		//Update
		bool update() override;
	};
}

#endif // !M_STATE_HPP