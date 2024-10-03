/*****************************************************************//**
 * \file   sysFont.h
 * \brief	Font render manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *********************************************************************/

#pragma once

#ifndef FONT_SYSTEM_HPP
#define FONT_SYSTEM_HPP

#include "../headers/Components/cRender.h"

namespace Font {

	//Font manager
	class Manager {
	private:
		//Free type lib
		FT_Library ft_lib;

		//Quad rendering
		unsigned int VAO, VBO;

		//Setup VAO & BAO
		void configQuadRendering();

		//Generate texture from glyphs for rendering
		std::unordered_map<unsigned char, Render::Character> generateGlyphsTex(FT_Face& font_face);

	public:
		//Default constructor
		Manager();

		//Load free type font
		std::unordered_map<unsigned char, Render::Character> loadFont(std::string const& file_path, Vector2 const& pixel_sizes = {0.0f, 48.0f});

		//Get VAO
		unsigned int getVAO() const;

		//Get VBO
		unsigned int getVBO() const;

		//Default destructor
		~Manager();
	};
}

#endif //!FONT_SYSTEM_HPP