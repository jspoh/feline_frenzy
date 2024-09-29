/*****************************************************************//**
 * \file   Camera.cpp
 * \brief  Implementation 
 *
 * \author g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/Render/Camera.h"



CameraObject::CameraObject() : position(Vector2(0, 0)), target(Vector2(0, 0)), up(Vector2(0, 1)), height(0), ar(0) {
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

void CameraObject::init(float wWidth, float wHeight, float camHeight) {

	height = camHeight;
	ar = static_cast<float>(wWidth) / wHeight;
	float angleDisp = 0 * PI / 180.f;

	up = Vector2(-sin(angleDisp), cos(angleDisp));
	target = Vector2(cos(angleDisp), sin(angleDisp));

	// Shifts camera to player
	view_xform = Matrix33::Matrix_33{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};

	// Converts worlds space coords to NDC
	cam_to_ndc_xform = Matrix33::Matrix_33{
		2.0f / ar / height, 0, 0,
		0, 2.0f / height, 0,
		0, 0, 1
	};
	world_to_ndc_xform = cam_to_ndc_xform * view_xform;
	
}

void CameraObject::update() {
	view_xform = Matrix33::Matrix_33{
	1, 0,  -position.x,
	0, 1,  -position.y,
	0, 0, 1
	};

	cam_to_ndc_xform = Matrix33::Matrix_33{
		2.0f / ar / height, 0, 0,
		0, 2.0f / height, 0,
		0, 0, 1
	};
	world_to_ndc_xform = cam_to_ndc_xform * view_xform;
}

void CameraObject::setPosition(float x, float y) {
	position = Vector2(x, y);
}

const Vector2& CameraObject::getPosition() const {
	return position;
}

const Matrix33::Matrix_33& CameraObject::getWorldToNDCXform() const
{ 
	return world_to_ndc_xform; 
};