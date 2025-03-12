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
		auto entity = NIKE_METADATA_SERVICE->getEntityByName(event->entity_name);
		if (entity.has_value() && NIKE_ECS_MANAGER->checkEntityComponent<Render::Cam>(entity.value())) {
			cam_name = event->entity_name;
		}
		else {
			if (event->fallback_cam != nullptr) {
				cam_name = event->entity_name;
				def_cam = event->fallback_cam;
			}
		}

		event->setEventProcessed(true);
	}

	/*****************************************************************//**
	* INITIALIZATION
	*********************************************************************/

	void Camera::Service::init(nlohmann::json const& config) {
		float angleDisp = 0 * static_cast<float>(M_PI) / 180.f;

		up = Vector2(-sin(angleDisp), cos(angleDisp));
		target = Vector2(cos(angleDisp), sin(angleDisp));
		cam_name = "Free Cam";
		cam_height = 0;

		//Setup events listening
		std::shared_ptr<Camera::Service> cam_sys_wrapped(this, [](Camera::Service*){});
		NIKE_EVENTS_SERVICE->addEventListeners<Render::ChangeCamEvent>(cam_sys_wrapped);


		// Camera config
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

	/*****************************************************************//**
	* TRANSFORMS
	*********************************************************************/

	Matrix_33 Camera::Service::getWorldToNDCXform(bool mouse_effect) const
	{
		Render::Cam cam;

		//Get entity from cam name
		auto cam_entity = NIKE_METADATA_SERVICE->getEntityByName(cam_name);

		//Check if camera entity exists
		if (cam_entity.has_value() && NIKE_ECS_MANAGER->checkEntity(cam_entity.value())) {

			//Get camp component
			auto e_cam_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_entity.value());
			if (e_cam_comp.has_value()) {
				
				//Check if camera attached to entity has a transform
				auto const& e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(cam_entity.value());

				//Apply transformation to camera position
				if (e_transform_comp.has_value()) e_cam_comp.value().get().position = e_transform_comp.value().get().position;

				//Set camera value
				cam = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_entity.value()).value();
			}
			else {

				//Check if camera attached to entity has a transform
				auto const& e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(cam_entity.value());

				//Apply transformation to camera position
				if (e_transform_comp.has_value()) {
					//Add camera component
					NIKE_ECS_MANAGER->addEntityComponent<Render::Cam>(cam_entity.value(), Render::Cam(e_transform_comp.value().get().position, cam_height));
				}
				else {
					//Add camera component
					NIKE_ECS_MANAGER->addEntityComponent<Render::Cam>(cam_entity.value(), Render::Cam(cam_height));
				}

				//Set camera value
				cam = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_entity.value()).value();
			}

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

		#ifndef NDEBUG
				if (!NIKE_LVLEDITOR_SERVICE->getGameState()) {
					return cam_to_ndc_xform * view_xform;
				}
		#endif

		// Apply mouse offset only if its not free cam
		if (mouse_effect && cam_name != "Free Cam" && cam.mouse_offset != 0.f) {
			NIKE_INPUT_SERVICE->setCrosshair(true);
			Vector2f mouse_pos = NIKE_INPUT_SERVICE->getMouseWindowPos();
			Vector2f screen_size = NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize();

			Vector2f mouse_move_offset = {
				(mouse_pos.x / screen_size.x - 0.5f) * -2.0f,
				(mouse_pos.y / screen_size.y - 0.5f) * 2.0f
			};

			// Scale down the effect 
			mouse_move_offset *= cam.mouse_offset;

			// Create a translation matrix for the offset
			Matrix_33 mouse_offset_matrix{
				1, 0, mouse_move_offset.x,
				0, 1, mouse_move_offset.y,
				0, 0, 1
			};

			// Apply the offset
			return mouse_offset_matrix * cam_to_ndc_xform * view_xform;
		}
		else {
			NIKE_INPUT_SERVICE->setCrosshair(false);
		}


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

	/*****************************************************************//**
	* CAMERA 
	*********************************************************************/

	std::optional<Entity::Type> Camera::Service::getActiveCamId() const {
		return NIKE_METADATA_SERVICE->getEntityByName(cam_name);
	}

	std::string Camera::Service::getActiveCamName() const {
		return cam_name;
	}

	void Camera::Service::setActiveCamName(std::string active_cam) {
		cam_name = active_cam;
	}

	Render::Cam Camera::Service::getActiveCamera() const {
		Render::Cam cam;

		//Get entity from cam name
		auto cam_entity = NIKE_METADATA_SERVICE->getEntityByName(cam_name);

		//Check if camera entity exists
		if (cam_entity.has_value() && NIKE_ECS_MANAGER->checkEntity(cam_entity.value())) {

			//Get camp component
			auto e_cam_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_entity.value());
			if (e_cam_comp.has_value()) {

				//Check if camera attached to entity has a transform
				auto const& e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(cam_entity.value());

				//Apply transformation to camera position
				if (e_transform_comp.has_value()) e_cam_comp.value().get().position = e_transform_comp.value().get().position;

				//Set camera value
				cam = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_entity.value()).value();
			}
			else {

				//Check if camera attached to entity has a transform
				auto const& e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(cam_entity.value());

				//Apply transformation to camera position
				if (e_transform_comp.has_value()) {
					//Add camera component
					NIKE_ECS_MANAGER->addEntityComponent<Render::Cam>(cam_entity.value(), Render::Cam(e_transform_comp.value().get().position, cam_height));
				}
				else {
					//Add camera component
					NIKE_ECS_MANAGER->addEntityComponent<Render::Cam>(cam_entity.value(), Render::Cam(cam_height));
				}

				//Set camera value
				cam = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(cam_entity.value()).value();
			}

		}
		else {
			cam = *def_cam.get();
		}

		return cam;
	}

	std::shared_ptr<Render::Cam> Camera::Service::getDefaultCamera() const {
		return def_cam;
	}

	void Camera::Service::setCameraHeight(float height) {
		cam_height = height;
	}

	float Camera::Service::getCameraHeight() const {
		return cam_height;
	}

	void Camera::Service::setMouseOffset(float offset) {
		mouse_offset = offset;
	}

	float Camera::Service::getMouseOffset() const {
		return mouse_offset;
	}


	/*****************************************************************//**
	* CAMERA ENTITIES
	*********************************************************************/

	const std::vector<std::pair<Entity::Type, std::string>>& Camera::Service::getCameraEntities() const {
		return cam_entities;
	}

	void Camera::Service::clearCameraEntities() {
		cam_entities.clear();
	}

	void  Camera::Service::emplaceCameraEntity(Entity::Type entity, const std::string& name) {
		cam_entities.emplace_back(entity, name);  // This will add the entity or update its name if it already exists
	}

	/*****************************************************************//**
	* SERIALISATION
	*********************************************************************/

	nlohmann::json Camera::Service::serializeCamera() const{
		nlohmann::json camera_data;

		camera_data["Active Cam ID"] = getActiveCamName();

		return camera_data;
	}

	void Camera::Service::deserializeCamera(nlohmann::json const& data) {

		setActiveCamName(data.value("Active Cam ID",""));

	}
}
