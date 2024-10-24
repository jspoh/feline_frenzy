/*****************************************************************//**
 * \file   cRender.h
 * \brief
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef C_RENDER_HPP
#define C_RENDER_HPP

#include "Core/stdafx.h"

namespace NIKESAURUS {
	namespace Render {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		struct Cam {
			std::string id;
			Vector2f position;	// Position of camera
			float height;	// represents how much of the world is visible vertically (zoom level).
		};

		struct Color {
			Vector3f color;
			float alpha;
		};

		struct Font {
			struct Character {
				unsigned int texture;	// Texture ID for the character
				Vector2f size;			// Size of the character
				Vector2f bearing;		// Offset from the baseline to the top-left of the character
				unsigned int advance;   // Horizontal offset to advance to the next character
			};

			std::unordered_map<unsigned char, Character> char_map;
		};

		struct Text {
			std::string font_ref;
			std::string text;
			Color color;
			Vector2f position;
			float scale;
		};

		struct Shape {
			std::string model_ref;
			Matrix_33 x_form;
			Color color;
		};

		struct Texture {
			std::string texture_ref;
			Matrix_33 x_form;
			Color color;
			Vector2f texture_size;	// Spritesheet size ( before mapping )
			Vector2f frame_size;		// x: 1 / frames in col,  y: 1 / frames in row
			Vector2f frame_index;	// frame 1: (0,0), frame 2: (1,0) ( topleft to bot right )
		};

		struct Model {

			unsigned int vaoid;
			unsigned int vboid;
			unsigned int eboid;

			unsigned int primitive_type;
			unsigned int draw_count;
		};

		struct Debug {
			// TEMP FIX FOR DEBUG MODE
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif // !C_RENDER_HPP
