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

		void init(const Object& camObj);
		void update(const Object& camObj);

		const Matrix33::Matrix_33& getWorldToNDCXform() const;

	private:
		Vector2 position;
		Vector2 target;
		Vector2 up;

		float height;
		float ar;

		Matrix33::Matrix_33 view_xform;

		Matrix33::Matrix_33 cam_to_ndc_xform;
		Matrix33::Matrix_33 world_to_ndc_xform;

};