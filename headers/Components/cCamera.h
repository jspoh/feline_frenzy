#pragma once

#ifndef C_CAMERA_HPP
#define C_CAMERA_HPP

#include "../headers/Core/stdafx.h"

namespace Camera {

	struct Camera {
		Vector2 position;	// Position of camera

		float height;	// represents how much of the world is visible vertically (zoom level).
		float ar;	// aspect ratio is the ratio of the window's width to its height.

		// Matrix transformation
		Matrix33::Matrix_33 world_to_ndc_xform;
	};
}

#endif // !C_CAMERA_HPP