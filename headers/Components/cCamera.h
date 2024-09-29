#pragma once

#ifndef C_CAMERA_HPP
#define C_CAMERA_HPP

#include "../headers/Core/stdafx.h"

namespace Camera {

	struct Camera {

		Vector2 position;	// Position of camera

		float height;	// represents how much of the world is visible vertically (zoom level).
	};
}

#endif // !C_CAMERA_HPP