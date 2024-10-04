/*****************************************************************//**
 * \file   sysFont.cpp
 * \brief	Font render manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/Render/sysFont.h"
#include "../headers/Core/Engine.h"

Font::Manager::Manager() {
	//Init free type library
	if (FT_Init_FreeType(&ft_lib)) {
		cerr << "Could not initialize FreeType library!" << endl;
	}
}

std::unordered_map<unsigned char, Render::Character> Font::Manager::generateGlyphsTex(FT_Face& font_face) {

	//Map of glyph textures
	std::unordered_map<unsigned char, Render::Character> texture_map;

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
		Render::Character character{
			texture,
			{static_cast<float>(font_face->glyph->bitmap.width), static_cast<float>(font_face->glyph->bitmap.rows)},
			{static_cast<float>(font_face->glyph->bitmap_left), static_cast<float>(font_face->glyph->bitmap_top)},
			static_cast<unsigned int>(font_face->glyph->advance.x)
		};

		texture_map[c] = character;
	}

	//Clean up
	//FT_Done_Face(font_face);

	//Return glpyh textures
	return texture_map;
}

std::unordered_map<unsigned char, Render::Character> Font::Manager::loadFont(std::string const& file_path, Vector2 const& pixel_sizes) {
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

Font::Manager::~Manager() {
	FT_Done_FreeType(ft_lib);
}
