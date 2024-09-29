/*****************************************************************//**
 * \file   Camera.h
 * \brief  
 *
 * \author g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/
#pragma once

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/Render/Object.h"

class CameraObject {

	public:
		CameraObject();

		void init(float wHeight, float wWidth, float camHeight);
		void update();

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