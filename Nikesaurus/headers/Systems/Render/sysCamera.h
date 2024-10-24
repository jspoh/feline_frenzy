/*****************************************************************//**
 * \file   sysCamera.h
 * \brief  
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef CAMERA_SYSTEM_HPP
#define CAMERA_SYSTEM_HPP

#include "Core/stdafx.h"
#include "Components/cRender.h"

namespace NIKESAURUS {
	namespace Camera {

		class System {
		public:
			System();

			//Init camera
			void init(float camera_height);

			//Update camera
			void update();

			//Get matrix
			const Matrix_33& getWorldToNDCXform() const;

			// Get Pos to World coordinates
			const Vector3f getPosToWorld(const Vector2f& pos) const;
			//Get cam pos
			const Vector2f& getCamPos() const;

			//Update camera entities
			void updateCameraEntities(std::set<Entity::Type>&& entities);

			//Track camera entity
			void trackCamEntity(std::string const& cam_identifier);

		private:
			//Camera entity
			std::set<Entity::Type> cam_entities;

			//Current tracked cam
			std::string cam_id;

			//Targets
			Vector2f target;
			Vector2f up;

			//Camera aspect ratio
			float aspect_ratio;

			//Camera height
			float cam_height;

			//Camera position
			Vector2f cam_pos;

			// Matrix transformation
			Matrix_33 view_xform;
			Matrix_33 cam_to_ndc_xform;
			Matrix_33 world_to_ndc_xform;
		};
	}
}

#endif // !CAMERA_SYSTEM_HPP