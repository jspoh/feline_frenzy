#pragma once

#ifndef C_TRANSFORM_HPP
#define C_TRANSFORM_HPP

#include "../headers/Core/stdafx.h"

namespace Transform {
	// Maybe a mesh struct?

	struct Transform {
		Vector2 position;
		Vector2 scale;
		float rotation;
	};

	struct Runtime_Transform {
		bool runtime_scale_up = false;
		bool runtime_rotate = false;
		bool runtime_scale_down = false;
	};



	struct Velocity {
		Vector2 velocity;
	};
}

#endif // !C_TRANSFORM_HPP