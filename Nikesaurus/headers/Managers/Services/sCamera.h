﻿/*****************************************************************//**
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
		private:
			//Targets
			Vector2f target;
			Vector2f up;

			//Active cam id
			std::string cam_name;

			//Default Camera
			std::shared_ptr<Render::Cam> def_cam;

			//Camera height
			float cam_height;

			//Camera movement offset
			float mouse_offset;

			//On change camera event
			void onEvent(std::shared_ptr<Render::ChangeCamEvent> event) override;

			//List of camera entities
			std::vector<std::pair<Entity::Type, std::string>> cam_entities;

		public:
			Service() = default;

			/*****************************************************************//**
			* INITIALIZATION
			*********************************************************************/

			//Init camera
			void init(nlohmann::json const& config);

			/*****************************************************************//**
			* TRANSFORMS
			*********************************************************************/

			//Get matrix
			Matrix_33 getWorldToNDCXform(bool mouse_effect = true) const;

			//Get fixed world
			Matrix_33 getFixedWorldToNDCXform() const;

			// Get Pos to World coordinates
			const Vector3f getPosToWorld(const Vector2f& pos) const;

			/*****************************************************************//**
			* CAMERA
			*********************************************************************/

			// Return active Cam id
			std::optional<Entity::Type> getActiveCamId() const;

			// Set active cam name
			void setActiveCamName(std::string active_cam);

			// Return active Cam name
			std::string getActiveCamName() const;

			//Get active camera
			Render::Cam getActiveCamera() const;

			// Get reference to the default camera
			std::shared_ptr<Render::Cam> getDefaultCamera() const;

			//Set camera height
			void setCameraHeight(float height);

			//Get camera height
			float getCameraHeight() const;

			//Set mouse offset
			void setMouseOffset(float offset);

			//Get camera offest
			float getMouseOffset() const;
			

			/*****************************************************************//**
			* CAMERA ENTITIES
			*********************************************************************/

			//Get camera entities
			const std::vector<std::pair<Entity::Type, std::string>>& getCameraEntities() const;

			// Emplace new entity or update existing camera entity
			void emplaceCameraEntity(Entity::Type entity, const std::string& name);

			// Clears the cam_entities map
			void clearCameraEntities();

			/*****************************************************************//**
			* SERIALISATION
			*********************************************************************/

			// Serialize
			nlohmann::json serializeCamera() const;

			// Deserialize
			void deserializeCamera(nlohmann::json const& data);

		};
	}
}

#endif // !CAMERA_SYSTEM_HPP