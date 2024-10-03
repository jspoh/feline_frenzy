/*****************************************************************//**
 * \file   sysRender.h
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \co-author Sean Gwee, g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef RENDER_MANAGER_HPP
#define RENDER_MANAGER_HPP

#include "../headers/Systems/Render/sysFont.h"
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

		//Font system
		std::unique_ptr<Font::Manager> font_system;

		//Shader system
		std::unique_ptr<Shader::Manager> shader_system;

		//Camera System
		std::unique_ptr<Camera::System> camera_system; // smart ptr to camera

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

		/**
		 * all .tex files should be 256x256 in RGBA8 format.
		 *
		 * \param path_to_texture
		 * \param [out] width
		 * \param [out] height
		 * \param [out] tex_size
		 * @param [out] is_tex_ext
		 *
		 * @returns dynamically allocated char*
		 */
		char* prepareImageData(const std::string& path_to_texture, int& width, int& height, int& size, bool& is_tex_or_png_ext);

		//Transform matrix
		void transformMatrix(Transform::Transform const& obj, Matrix33::Matrix_33& x_form, Matrix33::Matrix_33 world_to_ndc_mat);

		void transformMatrixDebug(Transform::Transform const& obj, Matrix33::Matrix_33& x_form, Matrix33::Matrix_33 world_to_ndc_mat);
		
		//Render Shape
		void renderObject(Render::Shape const& e_shape);

		//Render Texture
		void renderObject(Render::Texture const& e_texture);

		//Render text
		void renderText(Render::Text const& e_text);

		//Render debugging wireframe
		void renderWireFrame(Matrix33::Matrix_33 const& x_form, Render::Color const& e_color);

		//Helper function to encapsulate rendering
		void transformAndRenderEntity(Entity::Type entity, bool debugMode);
	public:
		//Constructor
		Manager() = default;

		//Destructor
		~Manager() = default;

		// Debug mode for bounding box
		bool debug_mode = false;

		std::string getSysName()
		{
			return "Render System";
		}


		//Singleton Of Manager Class
		static std::shared_ptr<Manager> getInstance() {
			static std::shared_ptr<Manager> instance{ std::make_shared<Manager>() };
			return instance;
		}

		//Register free type font
		std::unordered_map<unsigned char, Render::Character> registerFont(std::string const& file_path, Vector2 const& pixel_sizes = { 0.0f, 48.0f });

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
		std::shared_ptr<Render::Model> registerModel(const std::string& path_to_mesh);

		/**
		 * registers textures.
		 * 
		 * files ending in `.tex` are assumed to already be clean RGBA8 format
		 * other files are parsed with stb_image.h
		 * 
		 * \param texture_ref
		 * \param path_to_texture
		 */
		unsigned int registerTexture(const std::string& path_to_texture);

		//Register shader
		unsigned int registerShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path);

		//Track camera entity
		void trackCamEntity(std::string const& cam_identifier);

		//Get camera entity
		std::unique_ptr<Camera::System>& getCamEntity();

		/**
		* update all object's xform
		* */
		void init() override;

		/**
		* update all object's xform
		* */
		bool update() override;
	};
}

#endif // !RENDER_MANAGER_HPP
