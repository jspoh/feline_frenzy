/*****************************************************************//**
 * \file   Camera.cpp
 * \brief  Implementation 
 *
 * \author g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "stdafx.h"
#include "Camera.h"


Camera::Camera() : height(0), ar(0) {}

void Camera::init(const Object& camObj) {
	// !TODO get width and height from config file
	int wWidth = 1600, wHeight = 900;

	height = 1000;
	ar = static_cast<float>(wWidth) / wHeight;
	float angleDisp = camObj.getOrientation() * PI / 180;;

	up = Vector2(-sin(angleDisp), cos(angleDisp));
	target = Vector2(cos(angleDisp), sin(angleDisp));

	view_xform = Matrix33::Matrix_33{
		1, 0,  -camObj.getPosition().x,
		0, 1,  -camObj.getPosition().y,
		0, 0, 1
	};

	cam_to_ndc_xform = Matrix33::Matrix_33{
		2.0f / ar / height, 0, 0,
		0, 2.0f / height, 0,
		0, 0, 1
	};
	world_to_ndc_xform = cam_to_ndc_xform * view_xform;
	
}

void Camera::update(const Object& camObj) {
	view_xform = Matrix33::Matrix_33{
	1, 0,  -camObj.getPosition().x,
	0, 1,  -camObj.getPosition().y,
	0, 0, 1
	};

	cam_to_ndc_xform = Matrix33::Matrix_33{
		2.0f / ar / height, 0, 0,
		0, 2.0f / height, 0,
		0, 0, 1
	};
	world_to_ndc_xform = cam_to_ndc_xform * view_xform;
}

const Matrix33::Matrix_33& Camera::getWorldToNDCXform() const 
{ 
	return world_to_ndc_xform; 
};