/*****************************************************************//**
 * \file   sScene.h
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef M_SCENE_HPP
#define M_SCENE_HPP

#include "sSerialization.h"

namespace NIKE {
	namespace Scenes {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		const int MAXLAYERS = 64;

		//Forward declaration of Scene service for friending layer
		class Service;

		//Layer class
		class NIKE_API Layer {
		public:
			using LayerMask = std::bitset<MAXLAYERS>;
		private:
			
			//Friend of layer class
			friend class Service;

			//Layer mask
			LayerMask mask;

			//Layer mask id
			unsigned int id;

			//Layer state
			bool b_state;

			//Vector of entity in order
			std::vector<Entity::Type> entities;
		public:
			Layer() : b_state{ true }, id{ 0 } {}
			~Layer() = default;

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

			//Insert entity
			void insertEntity(Entity::Type entity);

			//Remove entity
			void removeEntity(Entity::Type entity);

			//Check entity
			bool checkEntity(Entity::Type entity);

			//Sort entities
			void sortEntitiesBasedOnMetaData();

			//Set entity order in layer
			void setEntityOrder(Entity::Type entity, size_t order_in_layer);

			//get entity order in layer
			size_t getEntityOrder(Entity::Type entity) const;

			//Get entities
			std::vector<Entity::Type> getEntitites() const;

			//Get entities size
			size_t getEntitiesSize() const;

			//Serialize layer
			nlohmann::json serialize() const;

			//Deserialize layer
			void deserialize(nlohmann::json const& data);
		};

		//Scene manager actions
		enum class NIKE_API Actions {
			CHANGE = 0,
			RESTART,
			PREVIOUS,
			RESET,
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

		class NIKE_API Service {
		private:

			//Curr scene
			std::string curr_scene;

			//Prev scene
			std::string prev_scene;

			//Scene event queue
			std::queue<Scenes::SceneEvent> event_queue;

			//Layers within scene
			std::vector<std::shared_ptr<Layer>> layers;

			//Init starting scene
			void initScene(std::string const& scene_id);

			//Change scene
			void changeScene(std::string const& scene_id);

			//Restart scene
			void restartScene();

			//Go To Previous scene
			void previousScene();
		public:
			Service() = default;
			~Service() = default;

			//Create layer
			std::shared_ptr<Layer> createLayer(int index = -1);

			//Get layer
			std::shared_ptr<Layer> getLayer(unsigned int layer_id);

			//Clear layers in curr scene
			void clearLayers();

			//Remove layer
			void removeLayer(unsigned int layer_id);

			//Check layer
			bool checkLayer(unsigned int layer_id);

			//Get layer count
			unsigned int getLayerCount() const;

			//Get all layers
			std::vector<std::shared_ptr<Layer>>& getLayers();

			//Queue scene event
			void queueSceneEvent(Scenes::SceneEvent&& event);

			//Set curr scene id
			void setCurrSceneID(std::string const& new_scene_id);

			//Get curr scene id
			std::string getCurrSceneID() const;

			//Get prev scene id
			std::string getPrevSceneID() const;

			//Reset scene
			void resetScene();

			//Init scene service
			void init();

			//Update scene event
			void update();
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif // !M_SCENE_HPP