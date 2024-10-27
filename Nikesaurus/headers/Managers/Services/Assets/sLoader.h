/*****************************************************************//**
 * \file   sysFont.h
 * \brief	Font render manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef LOADER_HPP
#define LOADER_HPP

#include "Components/cRender.h"

namespace NIKESAURUS {
	namespace Font {

		//Abstract font lib interface
		class IFontLib {
		private:
		public:
			IFontLib() = default;
			virtual ~IFontLib() = default;
		};

		#ifdef NIKE_BUILD_DLL //!EXPOSE TO ENGINE ONLY

		//Font Service
		class NIKEFontLib : public IFontLib {
		private:
			//Free type lib
			FT_Library ft_lib;

			//Generate texture from glyphs for rendering
			Render::Font generateGlyphsTex(std::string const& file_path, FT_Face& font_face);

		public:
			//Default constructor
			NIKEFontLib();

			//Load free type font
			Render::Font generateFont(std::string const& file_path, Vector2f const& pixel_sizes = { 0.0f, 48.0f });

			//Default destructor
			~NIKEFontLib();
		};

		#endif //!EXPOSE TO ENGINE ONLY

		//Font Service
		class Loader {
		private:
			std::shared_ptr<IFontLib> font_lib;
		public:
			Loader();

			//Get font lib
			std::shared_ptr<IFontLib> getFontLib() const;
		};
	}

	namespace Render {
		class Loader {
		private:
			/**
			 * creates a vertex array object for base opengl shaders.
			 *
			 * \param vertices
			 * \param indices
			 * \param model		vao will be stored here
			 */
			void createBaseBuffers(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices, Render::Model& model);

			/**
			 * creates a vertex array object for base opengl shaders.
			 *
			 * \param vertices
			 * \param indices
			 * \param model		vao will be stored here
			 */
			void createTextureBuffers(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices, const std::vector<Vector2f>& tex_coords, Render::Model& model);

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


		public:
			Loader() = default;

			/**
			 * compiles shader and adds to shader_programs.
			 *
			 * \param shader_ref	shader program's reference string
			 * \param vtx_path		path to vertex shader
			 * \param frag_path		path to fragment shader
			 */
			unsigned int compileShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path);
		
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
			Render::Model compileModel(const std::string& path_to_mesh);

			/**
			 * registers textures.
			 *
			 * files ending in `.tex` are assumed to already be clean RGBA8 format
			 * other files are parsed with stb_image.h
			 *
			 * \param texture_ref
			 * \param path_to_texture
			 */
			unsigned int compileTexture(const std::string& path_to_texture, int* out_width = nullptr, int* out_height = nullptr);
		};
	}
}

#endif //!LOADER_HPP