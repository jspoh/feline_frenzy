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

#include "sSerialization.h"

namespace NIKE {
	namespace Scenes {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		//Forward declaration of IScene for friending layer
		class IScene;

		//Layer class
		class NIKE_API Layer {
		public:
			using LayerMask = std::bitset<64>;
		private:
			//Friend of layer class
			friend class IScene;

			//Layer mask
			LayerMask mask;

			//Layer index
			unsigned int index;

			//Layer mask id
			unsigned int id;

			//Layer state
			bool b_state;
		public:
			Layer() : b_state{ true }, index{ 0 }, id{ 0 } {}
			~Layer() = default;

			//Get layer index
			unsigned int getLayerIndex() const;

			//Get layer name
			unsigned int getLayerID() const;

			//Set layer state
			void setLayerState(bool state);

			//Get layer state
			bool getLayerState() const;

			//Set layer mask
			void setLayerMask(unsigned int mask_id, bool state);

			//Get layer mask
			LayerMask getLayerMask() const;

			//Serialize layer
			nlohmann::json serialize() const;

			//Deserialize layer
			void deserialize(nlohmann::json const& data);
		};

		//Scene interface
		class NIKE_API IScene {
		private:
			//File path to scene
			std::string file_path;

			//Layers within scene
			std::unordered_map<unsigned int, std::shared_ptr<Layer>> layers_map;
			std::vector<std::shared_ptr<Layer>> layers;

			//Layer count
			unsigned int layer_count;
		public:
			//Default constructor
			IScene() : layer_count{ 0 }{}

			//Phases
			virtual void load() = 0;
			virtual void init() = 0;
			virtual void exit() = 0;
			virtual void unload() = 0;

			//Create layer
			std::shared_ptr<Layer> createLayer(int index = -1);

			//Get layer
			std::shared_ptr<Layer> getLayer(unsigned int layer_id);

			//Remove layer
			void removeLayer(unsigned int layer_id);

			//Check layer
			bool checkLayer(unsigned int layer_id);

			//Get layer count
			unsigned int getLayerCount() const;

			//Get all layers
			std::vector<std::shared_ptr<Layer>>& getLayers();

			//Default virtual destructor
			virtual ~IScene() = default;
		};

		//Scene manager actions
		enum class NIKE_API Actions {
			CHANGE = 0,
			RESTART,
			PREVIOUS,
			CLOSE
		};

		//Change Scene Event
		struct NIKE_API SceneEvent {
			Actions scene_action;
			std::string next_scene_id;

			//Constructor
			SceneEvent(Actions scene_action, std::string next_scene_id)
				: scene_action{ scene_action }, next_scene_id{ next_scene_id } {}
		};

		//Scenes manager
		class NIKE_API Service {
		private:
			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

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
			Service() = default;

			//Register scenes
			template<typename T>
			void registerScene(std::string const& scene_id) {

				//New scene
				std::shared_ptr<T> scene{ std::make_shared<T>() };

				//Construct scene obj
				scenes.emplace(std::piecewise_construct, std::forward_as_tuple(scene_id), std::forward_as_tuple(scene));

				//Create first layer for scene
				scenes.at(scene_id)->createLayer();

				//Default starting state will be the first state registered
				if (!curr_scene) {
					initScene(scene_id);
				}
			}

			//Get Current Scene
			std::shared_ptr<IScene> getCurrScene();

			//Get Current Scene ID
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