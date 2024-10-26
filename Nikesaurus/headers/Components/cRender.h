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

		struct Cam {
			std::string id;
			Vector2f position;	// Position of camera
			float height;	// represents how much of the world is visible vertically (zoom level).

			Cam() : id{ "" }, position(), height{ 0.0f } {}
		};

		//Color Define
		using Color = Vector4f;

		struct Font {
			struct Character {
				unsigned int texture;	// Texture ID for the character
				Vector2f size;			// Size of the character
				Vector2f bearing;		// Offset from the baseline to the top-left of the character
				unsigned int advance;   // Horizontal offset to advance to the next character

				Character() : texture{ 0 }, size(), bearing(), advance{ 0 } {}
				Character(unsigned int texture, Vector2f const& size, Vector2f const& bearing, unsigned int advance) : texture{ texture }, size{ size }, bearing{ bearing }, advance{ advance } {}
			};

			std::unordered_map<unsigned char, Character> char_map;
		};

		struct Text {
			std::string font_ref;
			std::string text;
			Color color;
			Vector2f position;
			float scale;

			Text() : font_ref{ "" }, text{ "" }, color(), position(), scale{ 1.0f } {}
		};

		struct Shape {
			std::string model_ref;
			Matrix_33 x_form;
			Color color;

			Shape() : model_ref{ "" }, x_form(), color() {}
		};

		struct Texture {
			std::string texture_ref;
			Matrix_33 x_form;
			Color color;
			Vector2f texture_size;	// Spritesheet size ( before mapping )
			Vector2f frame_size;		// x: 1 / frames in col,  y: 1 / frames in row
			Vector2f frame_index;	// frame 1: (0,0), frame 2: (1,0) ( topleft to bot right )

			Texture() : texture_ref{ "" }, x_form(), color(), texture_size(), frame_size(), frame_index() {}
		};

		struct Model {
			unsigned int vaoid;
			unsigned int vboid;
			unsigned int eboid;

			unsigned int primitive_type;
			unsigned int draw_count;

			Model() : vaoid{ 0 }, vboid{ 0 }, eboid{ 0 }, primitive_type{ 0 }, draw_count{ 0 } {}
		};
	}
}

#endif // !C_RENDER_HPP
