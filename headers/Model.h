/*****************************************************************//**
 * \file   Model.h
 * \brief  Model class that holds the VAO, draw count, and primitive type.
 *
 * \author g.boonxuensean
 * \date   September 2024
 *********************************************************************/

#pragma once

#include "stdafx.h"

class Model {
	public: 
		unsigned int vaoid;
		unsigned int vboid;
		unsigned int eboid;
		unsigned int draw_count;

		Model();
		~Model();

		bool loadMesh(const std::string& path_to_mesh);
		void draw() const;

	private:
		void createBuffers(const std::vector<Vector2>& vertices, const std::vector<unsigned int>& indices);
};