/*****************************************************************//**
 * \file   Camera.h
 * \brief  
 *
 * \author g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/
#pragma once

#include "stdafx.h"
#include "Object.h"

class Camera {

	public:
		Camera();

		void init(const Object& camObj, float wHeight, float wWidth);
		void update(const Object& camObj);

		void setPosition(float x, float y);
		const Vector2& getPosition() const;

		const Matrix33::Matrix_33& getWorldToNDCXform() const;

	private:
		Vector2 position;	// Position of camera
		Vector2 target;
		Vector2 up;

		float height;	// represents how much of the world is visible vertically (zoom level).
		float ar;	// aspect ratio is the ratio of the window's width to its height.

		// Matrix transformation
		Matrix33::Matrix_33 view_xform;
		Matrix33::Matrix_33 cam_to_ndc_xform;
		Matrix33::Matrix_33 world_to_ndc_xform;

};