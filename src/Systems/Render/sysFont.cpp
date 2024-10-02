/*****************************************************************//**
 * \file   sysFont.cpp
 * \brief	Font render manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/Render/sysFont.h"
#include "../headers/Core/Engine.h"

Font::Manager::Manager() {

	//Init free type library
	if (FT_Init_FreeType(&ft_lib)) {
		cerr << "Could not initialize FreeType library!" << endl;
	}

	//Config fonttype for quad rendering
	configQuadRendering();
}

void Font::Manager::configQuadRendering() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
}

std::unordered_map<unsigned char, Render::Character> Font::Manager::generateGlyphsTex(FT_Face& font_face) {

	std::unordered_map<unsigned char, Render::Character> texture_map;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(font_face, c, FT_LOAD_RENDER)) {
			cerr << "Failed to load glyph." << endl;
			continue;
		}

		//Font texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font_face->glyph->bitmap.width,
			font_face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
			font_face->glyph->bitmap.buffer);


		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Save character textures
		Render::Character character{ texture, 
			{(float)font_face->glyph->bitmap.width, (float)font_face->glyph->bitmap.rows},
			{(float)font_face->glyph->bitmap_left, (float)font_face->glyph->bitmap_top},
			(unsigned int)font_face->glyph->advance.x };

		texture_map.insert({ c,  character });
	}

	//Clean up
	FT_Done_Face(font_face);

	return texture_map;
}

std::unordered_map<unsigned char, Render::Character> Font::Manager::loadFont(std::string const& file_path, Vector2 const& pixel_sizes) {
	//Create free type share pointer
	FT_Face face;

	//Load font face
	if (FT_New_Face(ft_lib, file_path.c_str(), 0, &face)) {
		cerr << "Could not load font face!" << endl;
	}

	// Set the font size ( width and height of the glyph )
	FT_Set_Pixel_Sizes(face, (FT_UInt)pixel_sizes.x, (FT_UInt)pixel_sizes.y);

	//Generate vector of textures
	return generateGlyphsTex(face);
}

unsigned int Font::Manager::getVAO() const {
	return VAO;
}

unsigned int Font::Manager::getVBO() const {
	return VBO;
}

Font::Manager::~Manager() {
	FT_Done_FreeType(ft_lib);
}
