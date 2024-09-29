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
#include "../headers/Systems/Render/sysCamera.h"
#include "../headers/Components/cRender.h"
#include "../headers/Components/cTransform.h"
#include "../headers/Managers/mSystem.h"
#include "../headers/Core/Engine.h"

namespace Render {

	class Manager : public System::ISystem{

	private:

		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		//Shader system
		std::unique_ptr<Shader::Manager> shader_system;

		//Camera System
		std::unique_ptr<Camera::System> camera_system; // smart ptr to camera

		//Map to models for drawing
		std::unordered_map<std::string, std::shared_ptr<Render::Model>> models;

		// Map to textures for drawing
		std::unordered_map<std::string, unsigned int> textures;

		/**
		 * creates a vertex array object for base opengl shaders.
		 * 
		 * \param vertices
		 * \param indices
		 * \param model		vao will be stored here
		 */
		void createBaseBuffers(const std::vector<Vector2>& vertices, const std::vector<unsigned int>& indices, std::shared_ptr<Render::Model> model);

		/**
		 * creates a vertex array object for base opengl shaders.
		 *
		 * \param vertices
		 * \param indices
		 * \param model		vao will be stored here
		 */
		void createTextureBuffers(const std::vector<Vector2>& vertices, const std::vector<unsigned int>& indices, const std::vector<Vector2>& tex_coords, std::shared_ptr<Render::Model> model);

		//Load mesh from txt file
		bool loadMesh(const std::string& path_to_mesh, std::shared_ptr<Render::Model> model);

		//Transform matrix
		void transformMatrix(Transform::Transform& xform, Render::Mesh& mesh, Matrix33::Matrix_33 world_to_ndc_mat);

		//Render object
		void renderObject(Render::Mesh const& e_mesh, Render::Color const& e_color);

		//Render debugging wireframe
		void renderWireFrame(Render::Mesh const& e_mesh, Render::Color const& e_color);

		//Render object with texture
		void renderObject(const Render::Mesh& e_mesh);

		/**
		 * all .tex files should be 256x256 in RGBA8 format.
		 * 
		 * \param path_to_texture
		 * \param width
		 * \param height
		 * \param size
		 * 
		 * @returns dynamically allocated char*
		 */
		char* prepareImageData(const std::string& path_to_texture, int& width, int& height, int& size);

	public:
		//Constructor
		Manager() = default;

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
		 * `n` prefix indicates name attribute
		 * `v` prefix indicates vertex attribute
		 * `t` prefix indicates index(triangle) attribute. (indexed rendering with element buffer object)
		 * top of the file indicates vertex count, index count. int format.
		 *
		 * important to note that anticlockwise generated shapes are front facing and vice versa.
		 * back facing triangles will be culled.
		 *
		 * example square mesh:
			n square
			v 0.5 -0.5
			v 0.5 0.5
			v -0.5 0.5
			v -0.5 -0.5
			t 0 1 2
			t 2 3 0
		 *
		 *
		 * \param mesh_ref
		 * \param path_to_mesh
		 * \return success
		 */
		void registerModel(const std::string& model_ref, const std::string& path_to_mesh);

		/**
		 * registers textures.
		 * 
		 * files ending in `.tex` are assumed to already be clean RGBA8 format
		 * other files are parsed with stb_image.h
		 * 
		 * \param texture_ref
		 * \param path_to_texture
		 */
		void registerTexture(const std::string& texture_ref, const std::string& path_to_texture);

		//Track camera entity
		void trackCamEntity(std::string const& cam_identifier);

		/**
		* update all object's xform
		* */
		void init() override;

		/**
		* update all object's xform
		* */
		void update() override;
	};
}

#endif // !RENDER_MANAGER_HPP
