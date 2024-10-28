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

#include "Managers/Services/sEvents.h"
#include "Managers/Services/sWindows.h"
#include "Components/cRender.h"

namespace NIKESAURUS {
	namespace Camera {

		class System 
			:	public Events::IEventListener<Windows::WindowResized>,
			public Events::IEventListener<Render::ChangeCamEvent>
		{
		public:
			System();

			//Init camera
			void init();

			//Get matrix
			Matrix_33 getWorldToNDCXform() const;

			// Get Pos to World coordinates
			const Vector3f getPosToWorld(const Vector2f& pos) const;
		private:

			//Targets
			Vector2f target;
			Vector2f up;

			//Active cam id
			Entity::Type cam_id;

			//Default Camera
			Render::Cam def_cam;

			//Camera aspect ratio
			float aspect_ratio;

			//On window resized event
			void onEvent(std::shared_ptr<Windows::WindowResized> event) override;

			//On change camera event
			void onEvent(std::shared_ptr<Render::ChangeCamEvent> event) override;
		};
	}
}

#endif // !CAMERA_SYSTEM_HPP