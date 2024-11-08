/*****************************************************************//**
 * \file   sysCamera.cpp
 * \brief  Implementation of camera system
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Systems/Render/sysCamera.h"
#include "Core/Engine.h"

namespace NIKE {
	Camera::System::System() : target(Vector2f(0, 0)), up(Vector2f(0, 1)), cam_id{ 0 }, aspect_ratio{ 0.0f } { }

	void Camera::System::onEvent(std::shared_ptr<Windows::WindowResized> event) {
		def_cam.height = static_cast<float>(event->frame_buffer.y);
		aspect_ratio = static_cast<float>(event->frame_buffer.x) / static_cast<float>(event->frame_buffer.y);
	}

	void Camera::System::onEvent(std::shared_ptr<Render::ChangeCamEvent> event) {
		cam_id = event->entity_id;
		event->setEventProcessed(true);
	}

	void Camera::System::onEvent(std::shared_ptr<Render::UpdateCamEvent> event) {
		// !TODO remove hard coded values for the adjustments
		// Zoom Controls with Clamp
		const float min_zoom = 0.0f;
		const float max_zoom = 2000.0f;  

		Render::Cam& active_cam = (NIKE_ECS_MANAGER->checkEntity(cam_id) && NIKE_ECS_MANAGER->checkEntityComponent<Render::Cam>(cam_id)) 
			? NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_id) : def_cam;

		// !TODO remove hard coded values for the adjustments
		if (event->edit_position == NIKE::Render::CamPosition::UP) {
			active_cam.position.y += 5.f;
		}
		else if (event->edit_position == NIKE::Render::CamPosition::DOWN) {
			active_cam.position.y -= 5.f;
		}
		else if (event->edit_position == NIKE::Render::CamPosition::LEFT) {
			active_cam.position.x -= 5.f;
		}
		else if (event->edit_position == NIKE::Render::CamPosition::RIGHT) {
			active_cam.position.x += 5.f;
		}
		else if (event->edit_position == NIKE::Render::CamPosition::RESET_POS) {
			active_cam.position = Vector2f(0.f, 0.f);
		}

		if (event->edit_zoom == NIKE::Render::CamZoom::ZOOM_IN) {
			active_cam.height -= 10.f;
		}
		else if (event->edit_zoom == NIKE::Render::CamZoom::ZOOM_OUT) {
			active_cam.height += 10.f;
		}
		else if (event->edit_zoom == NIKE::Render::CamZoom::RESET_ZOOM) {
			active_cam.height = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);
		}
		// Clamp the zoom height
		active_cam.height = std::clamp(active_cam.height, min_zoom, max_zoom);

		event->setEventProcessed(true);
	}

	void Camera::System::init() {
		// !TODO set height as a constant from the config
		aspect_ratio = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x) / static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);
		float angleDisp = 0 * static_cast<float>(M_PI) / 180.f;

		up = Vector2(-sin(angleDisp), cos(angleDisp));
		target = Vector2(cos(angleDisp), sin(angleDisp));

		//Setup events listening
		std::shared_ptr<Camera::System> cam_sys_wrapped(this, [](Camera::System*){});
		NIKE_EVENTS_SERVICE->addEventListeners<Windows::WindowResized>(cam_sys_wrapped);
		NIKE_EVENTS_SERVICE->addEventListeners<Render::ChangeCamEvent>(cam_sys_wrapped);
		NIKE_EVENTS_SERVICE->addEventListeners<Render::UpdateCamEvent>(cam_sys_wrapped);

		//Setup default camera
		def_cam.position = { 0.0f, 0.0f };
		def_cam.height = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);
	}

	Entity::Type Camera::System::getCamId() const {
		return cam_id;
	}

	Matrix_33 Camera::System::getWorldToNDCXform() const
	{
		Render::Cam cam;
		if (NIKE_ECS_MANAGER->checkEntity(cam_id) && NIKE_ECS_MANAGER->checkEntityComponent<Render::Cam>(cam_id)) {
			if(NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(cam_id))
			NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_id).position = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(cam_id).position;

			cam = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_id);
		}
		else {
			cam = def_cam;
		}

		Matrix_33 view_xform {
			1, 0,  -cam.position.x,
			0, 1,  -cam.position.y,
			0, 0, 1
		};

		Matrix_33 cam_to_ndc_xform {
			2.0f / aspect_ratio / cam.height, 0, 0,
			0, 2.0f / cam.height, 0,
			0, 0, 1
		};

		return cam_to_ndc_xform * view_xform;
	};

	Matrix_33 Camera::System::getFixedWorldToNDCXform() const
	{
		//Default camera altributes
		Render::Cam def;
		def.position = { 0.0f, 0.0f };
		def.height = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);

		Matrix_33 view_xform{
			1, 0,  -def.position.x,
			0, 1,  -def.position.y,
			0, 0, 1
		};

		Matrix_33 cam_to_ndc_xform{
			2.0f / aspect_ratio / def.height, 0, 0,
			0, 2.0f / def.height, 0,
			0, 0, 1
		};

		return cam_to_ndc_xform * view_xform;
	};

	const Vector3f Camera::System::getPosToWorld(const Vector2f& pos) const
	{
		float ndcX = (2.0f * pos.x) / NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x - 1.0f;
		float ndcY = 1.0f - (2.0f * pos.y) / NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y;

		Matrix_33 view_xform{
			1, 0,  -def_cam.position.x,
			0, 1,  -def_cam.position.y,
			0, 0, 1
		};

		Matrix_33 cam_to_ndc_xform{
			2.0f / aspect_ratio / def_cam.height, 0, 0,
			0, 2.0f / def_cam.height, 0,
			0, 0, 1
		};

		Matrix_33 ndc_to_world_xform;
		Matrix_33Inverse(ndc_to_world_xform, cam_to_ndc_xform * view_xform);

		Vector3f ndc_coords = { ndcX, ndcY, 1.0f }; // NDC in homogeneous coordinates
		Vector3f world_coords = ndc_coords * ndc_to_world_xform;

		return world_coords;
	};
}
