/*****************************************************************//**
 * \file	sLoader.h
 * \brief	Loading manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef LOADER_HPP
#define LOADER_HPP

namespace NIKE {
	namespace Assets {

		// Vertex data structure
		struct Vertex {
			Vector2f pos{};
			Vector4f col{};
			Vector2f tex_coords{};
			float sampler_idx{};	// index to use in sampler2DArray
			Matrix_33 transform{};		// column major
			Vector2f framesize{};
			Vector2f uv_offset{};
			float to_blend_color{};		// used for texture rendering. is bool value
			float blend_intensity{};		// used for texture rendering

			//Vertex() : pos(), col(), tex_coords(), transform(), sampler_idx{} {}
			//Vertex(const Vector2f& pos) : pos{ pos }, col(), tex_coords(), transform(), sampler_idx{} {}
			//Vertex(const Vector2f& pos, const Matrix_33& transform) : pos{ pos }, col{}, tex_coords{}, transform{ transform }, sampler_idx{} {}
			//Vertex(const Vector2f& pos, const Vector4f& col) : pos{ pos }, col{ col }, tex_coords{}, transform{}, sampler_idx{} {}
			//Vertex(const Vector2f& pos, const Vector4f& col, const Matrix_33& transform) : pos{ pos }, col{ col }, tex_coords(), transform{ transform }, sampler_idx{} {}
			//Vertex(const Vector2f& pos, const Vector4f& col, const Vector2f& tex_coords, float sampler_idx, const Matrix_33& transform) : pos{ pos }, col{ col }, tex_coords{ tex_coords }, transform(transform), sampler_idx{ sampler_idx } {}
			//Vertex(const Vector2f& pos, const Vector4f& col, const Vector2f& tex_coords, float sampler_idx, const Matrix_33& transform, const Vector2f& framesize, const Vector2f& uv_offset) : pos{ pos }, col{ col }, tex_coords{ tex_coords }, transform{ transform }, sampler_idx{ sampler_idx }, framesize{ framesize }, uv_offset{ uv_offset } {}
		};

		//Font Type Data Structure
		struct Font {
			struct Character {
				unsigned int texture;
				Vector2f size;			// Size of the character
				Vector2f bearing;		// Offset from the baseline to the top-left of the character
				unsigned int advance;   // Horizontal offset to advance to the next character

				Character() : texture{ 0 }, size(), bearing(), advance{ 0 } {}
				Character(unsigned int texture, Vector2f const& size, Vector2f const& bearing, unsigned int advance)
					: texture{ texture }, size{ size }, bearing{ bearing }, advance{ advance } {}
			};

			std::unordered_map<unsigned char, Character> char_map;
		};

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
			Font generateGlyphsTex(std::string const& file_path, FT_Face& font_face);

		public:
			//Default constructor
			NIKEFontLib();

			//Load free type font
			Font generateFont(std::string const& file_path, Vector2f const& pixel_sizes = { 0.0f, 48.0f });

			//Default destructor
			~NIKEFontLib();
		};

#endif //!EXPOSE TO ENGINE ONLY

		//Font Loader
		class FontLoader {
		private:
			std::shared_ptr<IFontLib> font_lib;
		public:
			FontLoader();

			//Get font lib
			std::shared_ptr<IFontLib> getFontLib() const;
		};

		//Model data structure
		struct Model {
			unsigned int vaoid;
			unsigned int vboid;
			unsigned int eboid;

			std::vector<unsigned int> indices;
			unsigned int primitive_type;
			unsigned int draw_count;

			std::vector<Vertex> vertices;

			Model() : vaoid{ 0 }, vboid{ 0 }, eboid{ 0 }, primitive_type{ 0 }, draw_count{ 0 } {}
		};

		//Texture data structure
		struct Texture {
			unsigned int gl_data;
			Vector2i size;
			std::string file_path;

			Texture() : gl_data{ 0 }, size{}, file_path{ "" } {}
			Texture(unsigned int gl_data, Vector2i size, std::string file_path)
				: gl_data{ gl_data }, size{ std::move(size) }, file_path{ std::move(file_path) } {}
		};

		//Shader/Model/Texture Loader
		class RenderLoader {
		private:
			/**
			 * creates a vertex array object for base opengl shaders.
			 *
			 * \param vertices
			 * \param indices
			 * \param model		vao will be stored here
			 */
			void createBaseBuffers(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices, Model& model);


			void createBatchedBaseBuffers(Model& model);

			/**
			 * creates a vertex array object for base opengl shaders.
			 *
			 * \param vertices
			 * \param indices
			 * \param model		vao will be stored here
			 */
			void createTextureBuffers(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices, const std::vector<Vector2f>& tex_coords, Model& model);

			void createBatchedTextureBuffers(Model& model);


		public:
			RenderLoader() = default;
			~RenderLoader() = default;


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
			static unsigned char* prepareImageData(const std::string& path_to_texture, int& width, int& height, int& size, bool& is_tex_or_png_ext);


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
			Model compileModel(const std::string& path_to_mesh);

			/**
			 * registers textures.
			 *
			 * files ending in `.tex` are assumed to already be clean RGBA8 format
			 * other files are parsed with stb_image.h
			 *
			 * \param texture_id
			 * \param path_to_texture
			 */
			Texture compileTexture(const std::string& path_to_texture);
		};
	}
}

#endif //!LOADER_HPP