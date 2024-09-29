#pragma once

#ifndef C_TRANSFORM_HPP
#define C_TRANSFORM_HPP

#include "../headers/Core/stdafx.h"

namespace Transform {
	// Maybe a mesh struct?
	struct Mesh {
		// std::string shader_ref;
		// std::string model_ref;
		// texture shit
	};

	struct Transform {
		std::string shader_ref;
		std::string model_ref;

		Vector2 position;
		Vector2 scale;
		float rotation;
		Matrix33::Matrix_33 x_form;

		bool isCam;
	};

	struct Velocity {
		Vector2 velocity;
	};
}

#endif // !C_TRANSFORM_HPP