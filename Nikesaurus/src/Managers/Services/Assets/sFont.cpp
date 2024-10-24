/*****************************************************************//**
 * \file   sysFont.cpp
 * \brief	Font render manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/Assets/sFont.h"
#include "Core/Engine.h"

namespace NIKESAURUS {
	/*****************************************************************//**
	* NIKE FONT LIB
	*********************************************************************/
	Font::NIKFontLib::NIKFontLib() {
		//Init free type library
		if (FT_Init_FreeType(&ft_lib)) {
			cerr << "Could not initialize FreeType library!" << endl;
		}
	}

	Render::Font Font::NIKFontLib::generateGlyphsTex(FT_Face& font_face) {

		//Map of glyph textures
		Render::Font font;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		constexpr unsigned char MAX_ASCII = 128;

		for (unsigned char c{}; c < MAX_ASCII; c++) {
			if (FT_Load_Char(font_face, c, FT_LOAD_RENDER)) {
				cerr << "Failed to load glyph: " << c << "." << endl;
				continue;
			}

			// !TODO: check if font textures are freed

			//Font texture
			unsigned int texture;
			glCreateTextures(GL_TEXTURE_2D, 1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D,
				0,		// level
				GL_RED, // internal format, GL_RED because we only need one channel
				font_face->glyph->bitmap.width,
				font_face->glyph->bitmap.rows,
				0,		// border
				GL_RED, // format
				GL_UNSIGNED_BYTE, // type
				font_face->glyph->bitmap.buffer
			);

			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//Save character textures
			font.char_map[c] = {
				texture,
				{static_cast<float>(font_face->glyph->bitmap.width), static_cast<float>(font_face->glyph->bitmap.rows)},
				{static_cast<float>(font_face->glyph->bitmap_left), static_cast<float>(font_face->glyph->bitmap_top)},
				static_cast<unsigned int>(font_face->glyph->advance.x)
			};
		}

		//Clean up
		//FT_Done_Face(font_face);

		//Return glpyh textures
		return font;
	}

	Render::Font Font::NIKFontLib::generateFont(std::string const& file_path, Vector2f const& pixel_sizes) {
		//Create free type font face
		FT_Face face;

		//Load font face
		if (FT_New_Face(ft_lib, file_path.c_str(), 0, &face)) {
			cerr << "Could not load font face!" << endl;
		}

		// Set the font size ( width and height of the glyph )
		FT_Set_Pixel_Sizes(face, 0, static_cast<unsigned int>(pixel_sizes.y));

		//Generate vector of textures
		return generateGlyphsTex(face);
	}

	Font::NIKFontLib::~NIKFontLib() {
		FT_Done_FreeType(ft_lib);
	}

	/*****************************************************************//**
	* FONT SERVICE
	*********************************************************************/

	Font::Service::Service() {
		font_lib = std::make_shared<IFontLib>();
	}

	std::shared_ptr<Font::IFontLib> Font::Service::getFontLib() const {
		return font_lib;
	}
}
