/*****************************************************************//**
 * \file   sCamera.cpp
 * \brief  Implementation of camera service
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/sCamera.h"
#include "Core/Engine.h"

namespace NIKE {

	void Camera::Service::onEvent(std::shared_ptr<Render::ChangeCamEvent> event) {
		if (NIKE_ECS_MANAGER->checkEntity(event->entity_id) && NIKE_ECS_MANAGER->checkEntityComponent<Render::Cam>(event->entity_id)) {
			cam_id = event->entity_id;
		}
		else {
			if (event->fallback_cam != nullptr) {
				cam_id = event->entity_id;
				def_cam = event->fallback_cam;
			}
		}

		event->setEventProcessed(true);
	}

	void Camera::Service::init(nlohmann::json const& config) {
		float angleDisp = 0 * static_cast<float>(M_PI) / 180.f;

		up = Vector2(-sin(angleDisp), cos(angleDisp));
		target = Vector2(cos(angleDisp), sin(angleDisp));
		cam_id = 0;
		cam_name = "Free Cam";

		//Setup events listening
		std::shared_ptr<Camera::Service> cam_sys_wrapped(this, [](Camera::Service*){});
		NIKE_EVENTS_SERVICE->addEventListeners<Render::ChangeCamEvent>(cam_sys_wrapped);

		try {
			auto const& data = config.at("CameraConfig");
			cam_height = data.at("Height").get<float>();
			Vector2f pos;
			pos.fromJson(data.at("Position"));
			def_cam = std::make_shared<Render::Cam>(pos, data.at("Zoom").get<float>());
		}
		catch (const nlohmann::json::exception& e) {
			NIKEE_CORE_WARN(e.what());
			NIKEE_CORE_WARN("Camera config invalid! Reverting to default camera config");

			cam_height = 900.0f;
			def_cam = std::make_shared<Render::Cam>(Vector2f(0.0f, 0.0f), 1.0f);
		}
	}

	Entity::Type Camera::Service::getCamId() const {
		return cam_id;
	}

	std::string Camera::Service::getActiveCamName() const{
		return cam_name;
	}

	void Camera::Service::setActiveCamName(std::string active_cam) {
		cam_name = active_cam;
	}

	Matrix_33 Camera::Service::getWorldToNDCXform() const
	{
		Render::Cam cam;
		//Check if camera entity exists
		auto e_cam_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_id);
		if (NIKE_ECS_MANAGER->checkEntity(cam_id) && e_cam_comp.has_value()) {

			//Check if camera attached to entity has a transform
			auto const& e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(cam_id);

			//Apply transformation to camera position
			if(e_transform_comp.has_value()) e_cam_comp.value().get().position = e_transform_comp.value().get().position;

			//Set camera value
			cam = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_id).value();
		}
		else {
			cam = *def_cam.get();
		}

		Matrix_33 view_xform {
			1, 0,  -cam.position.x,
			0, 1,  -cam.position.y,
			0, 0, 1
		};

		Matrix_33 cam_to_ndc_xform {
			2.0f / NIKE_WINDOWS_SERVICE->getWindow()->getAspectRatio() / (cam_height * cam.zoom), 0, 0,
			0, 2.0f / (cam_height * cam.zoom), 0,
			0, 0, 1
		};

		return cam_to_ndc_xform * view_xform;
	};

	Matrix_33 Camera::Service::getFixedWorldToNDCXform() const
	{
		//Default camera altributes
		Render::Cam def;
		def.position = { 0.0f, 0.0f };
		def.zoom = 1.0f;

		Matrix_33 view_xform{
			1, 0,  -def.position.x,
			0, 1,  -def.position.y,
			0, 0, 1
		};

		Matrix_33 cam_to_ndc_xform{
			2.0f / NIKE_WINDOWS_SERVICE->getWindow()->getAspectRatio() / (cam_height * def.zoom), 0, 0,
			0, 2.0f / (cam_height * def.zoom), 0,
			0, 0, 1
		};

		return cam_to_ndc_xform * view_xform;
	};

	const Vector3f Camera::Service::getPosToWorld(const Vector2f& pos) const
	{
		float ndcX = (2.0f * pos.x) / NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x - 1.0f;
		float ndcY = 1.0f - (2.0f * pos.y) / NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y;

		Matrix_33 view_xform{
			1, 0,  -def_cam->position.x,
			0, 1,  -def_cam->position.y,
			0, 0, 1
		};

		Matrix_33 cam_to_ndc_xform{
			2.0f / NIKE_WINDOWS_SERVICE->getWindow()->getAspectRatio() / (cam_height * def_cam->zoom), 0, 0,
			0, 2.0f / (cam_height * def_cam->zoom), 0,
			0, 0, 1
		};

		Matrix_33 ndc_to_world_xform;
		Matrix_33Inverse(ndc_to_world_xform, cam_to_ndc_xform * view_xform);

		Vector3f ndc_coords = { ndcX, ndcY, 1.0f }; // NDC in homogeneous coordinates
		Vector3f world_coords = ndc_coords * ndc_to_world_xform;

		return world_coords;
	};

	Render::Cam Camera::Service::getActiveCamera() const {
		Render::Cam cam;
		//Check if camera entity exists
		auto e_cam_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_id);
		//Check if camera attached to entity has a transform
		auto const& e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(cam_id);
		if (NIKE_ECS_MANAGER->checkEntity(cam_id) && e_cam_comp.has_value()) {

			//Apply transformation to camera position
			if (e_transform_comp.has_value()) 
				e_cam_comp.value().get().position = e_transform_comp.value().get().position;

			//Set camera value
			cam = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_id).value();
		}
		else {
			cam = *def_cam.get();
		}

		return cam;
	}

	void Camera::Service::setCameraHeight(float height) {
		cam_height = height;
	}

	float Camera::Service::getCameraHeight() const {
		return cam_height;
	}

	const std::unordered_map<Entity::Type, std::string>& Camera::Service::getCameraEntities() const {
		return cam_entities;
	}

	void Camera::Service::clearCameraEntities() {
		cam_entities.clear();
	}

	void  Camera::Service::emplaceCameraEntity(Entity::Type entity, const std::string& name) {
		cam_entities.emplace(entity, name);  // This will add the entity or update its name if it already exists
	}


	nlohmann::json Camera::Service::serializeCamera() const{
		nlohmann::json camera_data;

		camera_data["Active Cam ID"] = getActiveCamName();

		return camera_data;
	}

	void Camera::Service::deserializeCamera(nlohmann::json const& data) {

		setActiveCamName(data["Active Cam ID"].get<std::string>());

	}
}
