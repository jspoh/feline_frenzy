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

	struct Shape {
		std::string model_ref;
		Matrix33::Matrix_33 x_form;
		Color color;
	};

	struct Texture {
		std::string texture_ref;
		Matrix33::Matrix_33 x_form;
		Color color;
		Vector2 frame_size; // x: 1 / frames in col,  y: 1 / frames in row
		Vector2 uv_offset;	// u: 0.0f - 1.0f,  v: 0.0f - 1.0f
	};

	struct Model {

		unsigned int vaoid;
		unsigned int vboid;
		unsigned int eboid;

		unsigned int primitive_type;
		unsigned int draw_count;
	};
}

#endif // !C_RENDER_HPP
