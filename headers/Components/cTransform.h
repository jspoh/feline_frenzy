#pragma once

#ifndef C_TRANSFORM_HPP
#define C_TRANSFORM_HPP

#include "../headers/Core/stdafx.h"

namespace Transform {

	struct Transform {
		Vector2 position;
		Vector2 scale;
		float rotation;
		Matrix33::Matrix_33 x_form;
	};

	struct Velocity {

	};
}

#endif // !C_TRANSFORM_HPP