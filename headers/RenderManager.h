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
#include "Engine.h"
#include "Model.h"
#include "Object.h"
#include "Camera.h"


class RenderManager {
private:
	RenderManager();
	~RenderManager();


	ShaderManager shaderManager;
	std::unique_ptr<Camera> camera;

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
	 * i 0,1,2,2,3,0
	 * 
	 * 
	 * \param mesh_ref
	 * \param path_to_mesh
	 * \return success
	 */
	void registerModel(const std::string& model_ref, const std::string& path_to_mesh);

	void initCamera(const std::string& object_ref);

	/**
	*  creates an object and places it into a map of objects
	*  object has 
	*  object_ref - key for map
	*  model_ref - which model/mesh its referencing
	*  color - color of object in Vec3
	*  position - position of object (NDC Range x-axis [-1, 1], y-axis[-1,1])
	*  scale - scale of object 
	*  rotation - how fast you want the object to spin degrees (-360f to 360f)
	*  orientation - where the object is oriented in degrees (-360f to 360f)
	* */
	void createObject(const std::string& object_ref, const std::string& model_ref, const Vector3& color = Vector3(0.f, 0.f, 0.f), const Vector2& position = Vector2(0.f,0.f), const Vector2& scale = Vector2(1.f, 1.f), float orientation = 0, float rotation = 0);
	/**
	* returns an object pointer from the object map
	* */
	Object* getObject(const std::string& object_ref);
	/**
	* update all object's xform
	* */
	void updateObjects();
	/**
	* calls all of objects draws
	* */
	void drawObjects();

};
