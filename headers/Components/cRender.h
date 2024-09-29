#pragma once

#ifndef C_RENDER_HPP
#define C_RENDER_HPP

#include "../headers/Core/stdafx.h"

namespace Render {

	struct Cam {

		std::string id;

		Vector2 position;	// Position of camera

		float height;	// represents how much of the world is visible vertically (zoom level).
	};

	struct Color {
		Vector3 color;
		float alpha;
	};

	struct Mesh {
		std::string shader_ref;
		std::string model_ref;
		Matrix33::Matrix_33 x_form;
	};

	struct Model {
		// !TODO add primitive_type
		unsigned int vaoid;
		unsigned int vboid;
		unsigned int eboid;
		unsigned int draw_count;
	};
}

#endif // !C_RENDER_HPP
