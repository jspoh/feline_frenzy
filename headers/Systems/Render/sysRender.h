/*****************************************************************//**
 * \file   RenderManager.h
 * \brief
 *
 * \author jings
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef RENDER_MANAGER_HPP
#define RENDER_MANAGER_HPP

#include "../headers/Systems/Render/sysShader.h"
#include "../headers/Systems/Render/Object.h"
#include "../headers/Systems/Render/Camera.h"
#include "../headers/Components/cRender.h"
#include "../headers/Components/cTransform.h"
#include "../headers/Components/cCamera.h"
#include "../headers/Managers/mSystem.h"
#include "../headers/Core/Engine.h"

namespace Render {

	class Manager : public System::ISystem{

	private:

		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		Shader::Manager shaderManager;
		std::unique_ptr<CameraObject> camera; // smart ptr to camera

		//Map to models for drawing
		std::unordered_map<std::string, std::shared_ptr<Render::Model>> models;

		//Create buffer
		void createBuffers(const std::vector<Vector2>& vertices, const std::vector<unsigned int>& indices, std::shared_ptr<Render::Model> model);

		//Load mesh from txt file
		bool loadMesh(const std::string& path_to_mesh, std::shared_ptr<Render::Model> model);

		//Transform matrix
		void transformMatrix(Transform::Transform& xform, Matrix33::Matrix_33 world_to_ndc_mat);

	public:
		//Constructor
		Manager();

		//Destructor
		~Manager();

		//Singleton Of Manager Class
		static std::shared_ptr<Manager> getInstance() {
			static std::shared_ptr<Manager> instance{ std::make_shared<Manager>() };
			return instance;
		}

		/**
		 * creates vertex array object. from mesh data and registers it to meshes.
		 *
		 * mesh data format: newline separated values for each vertex, each value is a float.
		 * `v` prefix indicates vertex attribute
		 * `i` prefix indicates index attribute. (indexed rendering with element buffer object)
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

		void initCamera(float camHeight);

		/**
		* update all object's xform
		* */
		void init() override;

		/**
		* update all object's xform
		* */
		void update() override;
		/**
		* calls all of objects draws
		* */
		void drawObjects();
	};
}

#endif // !RENDER_MANAGER_HPP
