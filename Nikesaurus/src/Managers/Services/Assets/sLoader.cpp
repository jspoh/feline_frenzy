/*****************************************************************//**
 * \file   sysFont.cpp
 * \brief	Font render manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/Assets/sLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "data/stb_image.h"

namespace NIKESAURUS {
	/*****************************************************************//**
	* NIKE FONT LIB
	*********************************************************************/
	Font::NIKFontLib::NIKFontLib() {
		//Init free type library
		if (FT_Init_FreeType(&ft_lib)) {
			cerr << "Could not initialize FreeType library!" << endl;
		}
	}

	Render::Font Font::NIKFontLib::generateGlyphsTex(FT_Face& font_face) {

		//Map of glyph textures
		Render::Font font;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		constexpr unsigned char MAX_ASCII = 128;

		for (unsigned char c{}; c < MAX_ASCII; c++) {
			if (FT_Load_Char(font_face, c, FT_LOAD_RENDER)) {
				cerr << "Failed to load glyph: " << c << "." << endl;
				continue;
			}

			// !TODO: check if font textures are freed

			//Font texture
			unsigned int texture;
			glCreateTextures(GL_TEXTURE_2D, 1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D,
				0,		// level
				GL_RED, // internal format, GL_RED because we only need one channel
				font_face->glyph->bitmap.width,
				font_face->glyph->bitmap.rows,
				0,		// border
				GL_RED, // format
				GL_UNSIGNED_BYTE, // type
				font_face->glyph->bitmap.buffer
			);

			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//Save character textures
			font.char_map[c] = {
				texture,
				{static_cast<float>(font_face->glyph->bitmap.width), static_cast<float>(font_face->glyph->bitmap.rows)},
				{static_cast<float>(font_face->glyph->bitmap_left), static_cast<float>(font_face->glyph->bitmap_top)},
				static_cast<unsigned int>(font_face->glyph->advance.x)
			};
		}

		//Clean up
		//FT_Done_Face(font_face);

		//Return glpyh textures
		return font;
	}

	Render::Font Font::NIKFontLib::generateFont(std::string const& file_path, Vector2f const& pixel_sizes) {
		//Create free type font face
		FT_Face face;

		//Load font face
		if (FT_New_Face(ft_lib, file_path.c_str(), 0, &face)) {
			cerr << "Could not load font face!" << endl;
		}

		// Set the font size ( width and height of the glyph )
		FT_Set_Pixel_Sizes(face, 0, static_cast<unsigned int>(pixel_sizes.y));

		//Generate vector of textures
		return generateGlyphsTex(face);
	}

	Font::NIKFontLib::~NIKFontLib() {
		FT_Done_FreeType(ft_lib);
	}

	/*****************************************************************//**
	* FONT LOADER
	*********************************************************************/
	Font::Loader::Loader() {
		font_lib = std::make_shared<IFontLib>();
	}

	std::shared_ptr<Font::IFontLib> Font::Loader::getFontLib() const {
		return font_lib;
	}

	/*****************************************************************//**
	* RENDER LOADER
	*********************************************************************/
	void Render::Loader::createBaseBuffers(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices, Render::Model& model) {
		// VBO (Vertex Buffer Object)
		glCreateBuffers(1, &model.vboid);
		glNamedBufferStorage(model.vboid,
			sizeof(Vector2f) * vertices.size(),
			nullptr, // nullptr means no data is transferred
			GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferSubData(model.vboid, 0, sizeof(Vector2f) * vertices.size(), vertices.data());

		// VAO (Vertex Array Object)
		glCreateVertexArrays(1, &model.vaoid);

		// Vertex Position Array
		glEnableVertexArrayAttrib(model.vaoid, 0); // vertex attribute index 0
		glVertexArrayVertexBuffer(model.vaoid, 0, model.vboid, 0, sizeof(Vector2f)); // buffer binding point 0
		glVertexArrayAttribFormat(model.vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(model.vaoid, 0, 0);

		// Create EBO
		glCreateBuffers(1, &model.eboid);
		glNamedBufferStorage(model.eboid, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayElementBuffer(model.vaoid, model.eboid);
	}

	void Render::Loader::createTextureBuffers(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices, const std::vector<Vector2f>& tex_coords, Render::Model& model) {
		// VBO (Vertex Buffer Object)
		glCreateBuffers(1, &model.vboid);
		glNamedBufferStorage(model.vboid,
			sizeof(Vector2f) * vertices.size() + sizeof(Vector2f) * tex_coords.size(),
			nullptr, // nullptr means no data is transferred
			GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferSubData(model.vboid, 0, sizeof(Vector2f) * vertices.size(), vertices.data());
		glNamedBufferSubData(model.vboid, sizeof(Vector2f) * vertices.size(), sizeof(Vector2f) * tex_coords.size(), tex_coords.data());

		// VAO (Vertex Array Object)
		glCreateVertexArrays(1, &model.vaoid);

		// Vertex Position Array
		glEnableVertexArrayAttrib(model.vaoid, 0); // vertex attribute index 0
		glVertexArrayVertexBuffer(model.vaoid, 0, model.vboid, 0, sizeof(Vector2f)); // buffer binding point 0
		glVertexArrayAttribFormat(model.vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(model.vaoid, 0, 0);

		// Vertex texture coordinates
		glEnableVertexArrayAttrib(model.vaoid, 2);
		glVertexArrayVertexBuffer(model.vaoid, 1, model.vboid, sizeof(Vector2f) * vertices.size(), sizeof(Vector2f));
		glVertexArrayAttribFormat(model.vaoid, 2, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(model.vaoid, 2, 1);

		// Create EBO
		glCreateBuffers(1, &model.eboid);
		glNamedBufferStorage(model.eboid, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayElementBuffer(model.vaoid, model.eboid);
	}

	char* Render::Loader::prepareImageData(const std::string& path_to_texture, int& width, int& height, int& tex_size, bool& is_tex_or_png_ext) {
		// find file type
		std::string junk, filetype;
		std::stringstream ss{ path_to_texture };
		std::getline(ss, junk, '.');
		std::getline(ss, filetype, '.');

		if (filetype == "tex") {
			is_tex_or_png_ext = true;

			width = 256;
			height = 256;

			std::ifstream texture_file{ path_to_texture, std::ios::binary | std::ios::ate };
			if (!texture_file.is_open()) {
				cerr << "Failed to open texture file: " << path_to_texture << endl;
				throw std::runtime_error("Failed to open texture file.");
			}

			// get tex_size of texture file
			tex_size = static_cast<int>(texture_file.tellg());

			// return to beginning of file
			texture_file.seekg(0, std::ios::beg);

			char* tex_data{ new char[tex_size] };

			// read tex data into ptr
			if (!texture_file.read(reinterpret_cast<char*>(tex_data), tex_size)) {
				cerr << "Failed to read texture file: " << path_to_texture << endl;
				throw std::runtime_error("Failed to read texture file.");
			}
			texture_file.close();

			return tex_data;
		}

		// is not .tex file
		if (filetype == "png") {
			is_tex_or_png_ext = true;
		}
		else {
			is_tex_or_png_ext = false;
		}

		int channels;
		stbi_set_flip_vertically_on_load(true);
		char* img_data = reinterpret_cast<char*>(stbi_load(path_to_texture.c_str(), &width, &height, &channels, 0));
		if (img_data == nullptr) {
			cerr << "Failed to load image data: " << path_to_texture << endl;
			throw std::runtime_error("Failed to load image data.");
		}

		tex_size = width * height * channels;

		char* data = new char[tex_size];
		for (int i{}; i < tex_size; i++) {
			data[i] = img_data[i];
		}
		stbi_image_free(img_data);

		return data;
	}

	unsigned int Render::Loader::compileShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path) {
		// read and compile vertex shader
		std::ifstream vtx_file{ vtx_path };
		if (!vtx_file.is_open()) {
			cerr << "Failed to open vertex shader file: " << vtx_path << endl;
			throw std::exception();
		}

		std::stringstream vtx_buffer;
		vtx_buffer << vtx_file.rdbuf();
		vtx_file.close();
		const std::string vtx_str = vtx_buffer.str();
		const char* vtx_src = vtx_str.c_str();

		unsigned int vtx_handle = glCreateShader(GL_VERTEX_SHADER);
		if (!vtx_handle) {
			cerr << "Failed to create vertex shader program " << shader_ref << endl;
			throw std::exception();
		}
		glShaderSource(vtx_handle, 1, &vtx_src, nullptr);
		glCompileShader(vtx_handle);

		// read and compile fragment shader
		std::ifstream frag_file{ frag_path };
		if (!frag_file.is_open()) {
			cerr << "Failed to open fragment shader file: " << frag_path << endl;
			throw std::exception();
		}

		std::stringstream frag_buffer;
		frag_buffer << frag_file.rdbuf();
		frag_file.close();
		const std::string frag_str = frag_buffer.str();
		const char* frag_src = frag_str.c_str();

		unsigned int frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
		if (!frag_handle) {
			cerr << "Failed to create fragment shader program " << shader_ref << endl;
			throw std::exception();
		}
		glShaderSource(frag_handle, 1, &frag_src, nullptr);
		glCompileShader(frag_handle);

		// link shaders
		unsigned int shader_handle = glCreateProgram();
		if (!shader_handle) {
			cerr << "Failed to create shader program " << shader_ref << endl;
			throw std::exception();
		}

		glAttachShader(shader_handle, vtx_handle);
		glAttachShader(shader_handle, frag_handle);
		glLinkProgram(shader_handle);


		// validate shader program
		int success = false;
		glGetProgramiv(shader_handle, GL_LINK_STATUS, &success);

		if (!success) {
			char info_log[512];
			glGetProgramInfoLog(shader_handle, 512, nullptr, info_log);
			cerr << "Failed to link shader program " << shader_ref << ": " << info_log << endl;
			throw std::exception();
		}

		// cleanup shaders
		glDeleteShader(vtx_handle);
		glDeleteShader(frag_handle);

		return shader_handle;
	}

	Render::Model Render::Loader::compileMesh(const std::string& path_to_mesh) {
		Render::Model model;

		std::ifstream mesh_file{ path_to_mesh, std::ios::in };
		if (!mesh_file.is_open()) {
			throw std::exception("Failed to open mesh file.");
		}

		mesh_file.seekg(0, std::ios::beg);

		std::string line;
		GLshort index;

		// pos
		std::vector<Vector2f> vertices;
		std::vector<Vector2f> tex_coords;

		// indices (indexed rendering with element buffer object)
		std::vector<unsigned int> indices;

		// line data type (eg. vertex, color, indices)
		char type;

		while (getline(mesh_file, line)) {
			std::istringstream line_sstm{ line };

			line_sstm >> type;

			switch (type) {
			case 'n': {// name
				break;
			}
			case 'v': {// vertex
				float ndc_x, ndc_y;
				line_sstm >> ndc_x >> ndc_y;
				vertices.emplace_back(ndc_x, ndc_y);
				break;
			}
			case 't': {// triangle indices
				if (model.primitive_type == 0) {
					model.primitive_type = GL_TRIANGLES;
				}
				while (line_sstm >> index) { // Grab index position
					indices.emplace_back(index);
				}
				break;
			}
			case 'i': {
				float tex_x, tex_y;
				line_sstm >> tex_x >> tex_y;
				tex_coords.emplace_back(tex_x, tex_y);
				break;
			}
			case 'f': {// fan indices
				if (model.primitive_type == 0) {
					model.primitive_type = GL_TRIANGLE_FAN;
				}
				while (line_sstm >> index) { // Grab index position
					indices.emplace_back(index);
				}
				break;
			}
			default:
				std::string error = "Unknown data type in mesh file: " + type;
				throw std::exception(error.c_str());
			}
		}
		mesh_file.close();

		if (tex_coords.size() == 0) {
			createBaseBuffers(vertices, indices, model);
		}
		else {
			createTextureBuffers(vertices, indices, tex_coords, model);
		}
		model.draw_count = static_cast<GLuint>(indices.size());

		return model;
	}

	unsigned int Render::Loader::compileTexture(const std::string& path_to_texture) {
		// find file type
		std::string junk, filetype;
		std::stringstream ss{ path_to_texture };
		std::getline(ss, junk, '.');
		std::getline(ss, filetype, '.');

		int tex_width, tex_height, tex_size;
		bool is_tex_or_png_ext = false;
		const char* tex_data = prepareImageData(path_to_texture, tex_width, tex_height, tex_size, is_tex_or_png_ext);

		// create texture
		unsigned int tex_id;
		glCreateTextures(GL_TEXTURE_2D, 1, &tex_id);
		glTextureStorage2D(tex_id, 1, GL_RGBA8, tex_width, tex_height);
		glTextureSubImage2D(tex_id, 0, 0, 0, tex_width, tex_height, (is_tex_or_png_ext ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, tex_data);

		// no longer needed
		delete[] tex_data;

		cout << "Successfully registered texture from " << path_to_texture << endl;

		// Return texture
		return tex_id;
	}
}
