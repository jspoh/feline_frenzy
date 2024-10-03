/*****************************************************************//**
 * \file   cRender.h
 * \brief
 *
 * \author Sean Gwee, g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/
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

	struct Character {
		unsigned int texture;	// Texture ID for the character
		Vector2 size;			// Size of the character
		Vector2 bearing;		// Offset from the baseline to the top-left of the character
		unsigned int advance;   // Horizontal offset to advance to the next character
	};

	struct Text {
		std::string font_ref;
		std::string text;
		Color color;
		Vector2 position;
		float scale;
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
		Vector2 frame_size;		// x: 1 / frames in col,  y: 1 / frames in row
		Vector2 frame_index;	// frame 1: (0,0), frame 2: (1,0) ( topleft to bot right )
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
