/*****************************************************************//**
 * \file   RenderManager.h
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#pragma once

#include "stdafx.h"
#include "ShaderManager.h"
#include "Model.h"
#include "Object.h"


class RenderManager {
private:
	RenderManager();
	~RenderManager();


	ShaderManager shaderManager;

public:

	// vertex array objects + data required for drawing
	std::unordered_map<std::string, Model*> models;
	std::unordered_map<std::string, Object> objects;

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
	void registerModel(const std::string& model_ref, const std::string& path_to_mesh);
	void registerObject(const std::string& object_ref, const std::string& model_ref);

	void drawObjects();

};
