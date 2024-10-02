/*****************************************************************//**
 * \file   sysCamera.cpp
 * \brief  Implementation of camera system
 *
 * \author g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/Render/sysCamera.h"
#include "../headers/Core/Engine.h"

Camera::System::System() : target(Vector2(0, 0)), up(Vector2(0, 1)) {
	// Set default matrices as identity
	view_xform = Matrix33::Matrix_33{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};

	cam_to_ndc_xform = Matrix33::Matrix_33{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};

	world_to_ndc_xform = cam_to_ndc_xform * view_xform;
}

void Camera::System::init(float camera_height) {
	// !TODO set height as a constant from the config
	aspect_ratio = NIKEEngine.accessWindow()->getWindowSize().x / NIKEEngine.accessWindow()->getWindowSize().y;
	cam_height = camera_height;
	cam_pos = { 0.0f, 0.0f };
	float angleDisp = 0 * static_cast<float>(M_PI) / 180.f;

	up = Vector2(-sin(angleDisp), cos(angleDisp));
	target = Vector2(cos(angleDisp), sin(angleDisp));

	// Shifts camera to player
	view_xform = Matrix33::Matrix_33{
		1, 0, -cam_pos.x,
		0, 1, -cam_pos.y,
		0, 0, 1
	};

	// Converts worlds space coords to NDC
	cam_to_ndc_xform = Matrix33::Matrix_33{
		2.0f / aspect_ratio / cam_height, 0, 0,
		0, 2.0f / cam_height, 0,
		0, 0, 1
	};
	world_to_ndc_xform = cam_to_ndc_xform * view_xform;
}

void Camera::System::update() {

	//Tracked camera entities
	if (!cam_entities.empty()) {

		//Find camera
		for (auto const& cam : cam_entities) {
			auto& e_camera = NIKEEngine.getEntityComponent<Render::Cam>(cam);
			if (cam_id == e_camera.id) {
				//Assign found camera values
				cam_pos = e_camera.position;
				cam_height = e_camera.height;
			}
		}
	}

	//Update aspect ratio
	aspect_ratio = NIKEEngine.accessWindow()->getWindowSize().x / NIKEEngine.accessWindow()->getWindowSize().y;

	//Update matrix
	view_xform = Matrix33::Matrix_33{
	1, 0,  -cam_pos.x,
	0, 1,  -cam_pos.y,
	0, 0, 1
	};

	cam_to_ndc_xform = Matrix33::Matrix_33{
		2.0f / aspect_ratio / cam_height, 0, 0,
		0, 2.0f / cam_height, 0,
		0, 0, 1
	};
	world_to_ndc_xform = cam_to_ndc_xform * view_xform;
}

const Matrix33::Matrix_33& Camera::System::getWorldToNDCXform() const
{ 
	return world_to_ndc_xform; 
};

const Vector3 Camera::System::getPosToWorld(const Vector2& pos) const
{
	float ndcX = (2.0f * pos.x) / NIKEEngine.accessWindow()->getWindowSize().x - 1.0f;
	float ndcY = 1.0f - (2.0f * pos.y) / NIKEEngine.accessWindow()->getWindowSize().y;

	Matrix33::Matrix_33 ndc_to_world_xform;
	Matrix33::Matrix_33Inverse(ndc_to_world_xform, world_to_ndc_xform);

	Vector3 ndc_coords = { ndcX, ndcY, 1.0f }; // NDC in homogeneous coordinates
	Vector3 world_coords = ndc_coords * ndc_to_world_xform;

	return world_coords;
};


const Vector2& Camera::System::getCamPos() const {
	return cam_pos;
}

void Camera::System::updateCameraEntities(std::set<Entity::Type>&& entities) {
	cam_entities = std::move(entities);
}

void Camera::System::trackCamEntity(std::string const& cam_identifier) {
	cam_id = cam_identifier;
}
