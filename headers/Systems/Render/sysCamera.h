/*****************************************************************//**
 * \file   sysCamera.h
 * \brief  
 *
 * \author Sean Gwee, g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/
#pragma once

#ifndef CAMERA_SYSTEM_HPP
#define CAMERA_SYSTEM_HPP

#include "../headers/Core/stdafx.h"
#include "../headers/Components/cRender.h"

namespace Camera {

	class System {
	public:
		System();

		//Init camera
		void init(float camera_height);

		//Update camera
		void update();

		//Get matrix
		const Matrix33::Matrix_33& getWorldToNDCXform() const;

		// Get Pos to World coordinates
		const Vector3 getPosToWorld(const Vector2& pos) const;
		//Get cam pos
		const Vector2& getCamPos() const;

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
		Vector2 target;
		Vector2 up;

		//Camera aspect ratio
		float aspect_ratio;

		//Camera height
		float cam_height;

		//Camera position
		Vector2 cam_pos;

		// Matrix transformation
		Matrix33::Matrix_33 view_xform;
		Matrix33::Matrix_33 cam_to_ndc_xform;
		Matrix33::Matrix_33 world_to_ndc_xform;
	};
}

#endif // !CAMERA_SYSTEM_HPP