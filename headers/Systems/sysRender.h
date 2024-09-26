/*****************************************************************//**
 * \file   RenderManager.h
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#pragma once

#include "../headers/Core/stdafx.h"


// !TODO: move this code
struct Model {
	unsigned int vaoid;
	unsigned int primitive_type;
	unsigned int draw_count;	// number of vertices to draw
};


class RenderManager {
private:
	RenderManager();
	~RenderManager();

	std::unordered_map<std::string, unsigned int> shader_programs;

	/**
	 * compiles shader and adds to shader_programs.
	 *
	 * \param shader_ref	shader program's reference string
	 * \param vtx_path		path to vertex shader
	 * \param frag_path		path to fragment shader
	 */
	void compileShader(std::string shader_ref, std::string vtx_path, std::string frag_path);

	// vertex array objects + data required for drawing
	std::unordered_map<std::string, Model> models;

public:
	static RenderManager& getInstance();

	/**
	 * creates vertex array object. from mesh data and registers it to meshes.
	 * 
	 * mesh data format: newline separated values for each vertex, each value is a float.
	 * `v` prefix indicates vertex attribute
	 * `i` prefix indicates index attribute. (indexed rendering with element buffer object)
	 * `c` prefix indicates color attribute. (optional)
	 * top of the file indicates vertex count, index count. int format.
	 * 
	 * important to note that anticlockwise generated shapes are front facing and vice versa.
	 * back facing triangles will be culled.
	 * 
	 * example square mesh:
	 * 4,6
	 * v 0.5,-0.5
	 * v 0.5,0.5
	 * v -0.5,0.5
	 * v -0.5,-0.5
	 * c 1.0,0.0,0.0,1.0
	 * c 0.0,1.0,0.0,1.0
	 * c 0.0,0.0,1.0,1.0
	 * c 1.0,1.0,1.0,1.0
	 * i 0,1,2,2,3,0
	 * 
	 * 
	 * \param mesh_ref
	 * \param path_to_mesh
	 * \return success
	 */
	bool registerMesh(std::string mesh_ref, std::string path_to_mesh);

	void useShader(std::string shader_ref);

	void unuseShader();

	void drawModel(std::string mdl_ref);
};
