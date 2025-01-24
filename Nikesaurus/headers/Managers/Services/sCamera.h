/*****************************************************************//**
 * \file   sCamera.h
 * \brief  
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef CAMERA_SYSTEM_HPP
#define CAMERA_SYSTEM_HPP

#include "Managers/Services/sEvents.h"
#include "Managers/Services/sWindows.h"
#include "Components/cRender.h"

namespace NIKE {
	namespace Camera {

		class Service
			: public Events::IEventListener<Render::ChangeCamEvent>
		{
		public:
			Service();

			//Init camera
			void init(nlohmann::json const& config);

			// Return active Cam id
			Entity::Type getCamId() const;
			
			void setActiveCamName(std::string active_cam);

			// Return active Cam name
			std::string getActiveCamName() const;

			//Get matrix
			Matrix_33 getWorldToNDCXform() const;

			//Get fixed world
			Matrix_33 getFixedWorldToNDCXform() const;

			// Get Pos to World coordinates
			const Vector3f getPosToWorld(const Vector2f& pos) const;

			//Get active camera
			Render::Cam getActiveCamera() const;

			//Set camera height
			void setCameraHeight(float height);

			//Get camera height
			float getCameraHeight() const;
			
			//Get camera entities
			const std::unordered_map<Entity::Type, std::string>& getCameraEntities() const;

			// Emplace new entity or update existing camera entity
			void emplaceCameraEntity(Entity::Type entity, const std::string& name);

			// Clears the cam_entities map
			void clearCameraEntities();

			// Serialize
			nlohmann::json serializeCamera() const;

			// Deserialize
			void deserializeCamera(nlohmann::json const& data);

		private:
			//Targets
			Vector2f target;
			Vector2f up;

			//Active cam id
			Entity::Type cam_id;
			std::string cam_name;

			//Default Camera
			std::shared_ptr<Render::Cam> def_cam;

			//Camera height
			float cam_height;

			//On change camera event
			void onEvent(std::shared_ptr<Render::ChangeCamEvent> event) override;

			//List of camera entities
			std::unordered_map<Entity::Type, std::string> cam_entities;
		};
	}
}

#endif // !CAMERA_SYSTEM_HPP