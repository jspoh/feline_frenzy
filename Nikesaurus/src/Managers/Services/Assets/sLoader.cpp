/*****************************************************************//**
 * \file	sLoader.cpp
 * \brief	Loader manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu
 * \date   October 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/Assets/sLoader.h"
#include "Systems/Render/sysRender.h"

#define STB_IMAGE_IMPLEMENTATION
#include "data/stb_image.h"

namespace NIKE {
	/*****************************************************************//**
	* NIKE FONT LIB
	*********************************************************************/
	Assets::NIKEFontLib::NIKEFontLib() {
		//Init free type library
		if (FT_Init_FreeType(&ft_lib)) {
			cerr << "Could not initialize FreeType library!" << endl;
		}

		NIKEE_CORE_INFO("Free Type init success");
	}

	Assets::Font Assets::NIKEFontLib::generateGlyphsTex(std::string const& file_path, FT_Face& font_face) {

		//Return font
		Assets::Font font;

		//Set pixels
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		//Max ascii characters
		const int max_ascii = 128;

		//Get texture for each character
		for (unsigned char c = 0; c < max_ascii; c++)
		{
			//Load character glyph
			if (FT_Load_Char(font_face, c, FT_LOAD_RENDER))
			{
				cout << "Error loading glpyh for character: " << c << endl;
			}

			//Generate texture for character
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				font_face->glyph->bitmap.width,
				font_face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				font_face->glyph->bitmap.buffer
			);

			//Set texture
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//Store character variables
			font.char_map[c] = {
				texture,
				{static_cast<float>(font_face->glyph->bitmap.width), static_cast<float>(font_face->glyph->bitmap.rows)},
				{static_cast<float>(font_face->glyph->bitmap_left), static_cast<float>(font_face->glyph->bitmap_top)},
				static_cast<unsigned int>(font_face->glyph->advance.x)
			};
		}

		//Clean up
		FT_Done_Face(font_face);
		font_face = nullptr;

		//Log
		NIKEE_CORE_INFO("Sucessfully loaded font from " + file_path);

		//Return glpyh textures
		return font;
	}

	Assets::Font Assets::NIKEFontLib::generateFont(std::string const& file_path, Vector2f const& pixel_sizes) {
		//Create free type font face
		FT_Face face;

		//Load font face
		if (FT_New_Face(ft_lib, file_path.c_str(), 0, &face)) {
			cerr << "Could not load font face!" << endl;
		}

		// Set the font size ( width and height of the glyph )
		FT_Set_Pixel_Sizes(face, 0, static_cast<unsigned int>(pixel_sizes.y));

		//Generate vector of textures
		return generateGlyphsTex(file_path, face);
	}

	Assets::NIKEFontLib::~NIKEFontLib() {
		FT_Done_FreeType(ft_lib);
	}

	/*****************************************************************//**
	* FONT LOADER
	*********************************************************************/
	Assets::FontLoader::FontLoader() {
		font_lib = std::make_shared<NIKEFontLib>();
	}

	std::shared_ptr<Assets::IFontLib> Assets::FontLoader::getFontLib() const {
		return font_lib;
	}

	/*****************************************************************//**
	* RENDER LOADER
	*********************************************************************/

	void Assets::RenderLoader::createBaseBuffers(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices, Assets::Model& model) {
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

	void Assets::RenderLoader::createClusterParticleBuffers(unsigned int& vao, unsigned int& vbo) {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

		glCreateVertexArrays(1, &vao);
		glCreateBuffers(1, &vbo);

		// bind vbo to vao
		static constexpr int VBO_BINDING_INDEX = 10;
		static constexpr int PARTICLE_SIZE = sizeof(NIKE::SysParticle::Particle);
		glVertexArrayVertexBuffer(vao, VBO_BINDING_INDEX, vbo, 0, PARTICLE_SIZE);

		// allocate space for vbo
		static constexpr int MAX_VBO_SIZE = NIKE::SysParticle::MAX_GLOBAL_ACTIVE_PARTICLES * PARTICLE_SIZE;
		glNamedBufferStorage(vbo, MAX_VBO_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// set vertex array attributes

		// a_particle_position
		glEnableVertexArrayAttrib(vao, 0);
		glVertexArrayAttribFormat(
			vao,
			0,		// attrib idx
			2,		// size
			GL_FLOAT,
			false,
			offsetof(NIKE::SysParticle::Particle, pos)
		);
		glVertexArrayAttribBinding(vao, 0, VBO_BINDING_INDEX);

		// a_particle_color
		glEnableVertexArrayAttrib(vao, 1);
		glVertexArrayAttribFormat(
			vao,
			1,		// attrib idx
			4,		// size
			GL_FLOAT,
			false,
			offsetof(NIKE::SysParticle::Particle, color)
		);
		glVertexArrayAttribBinding(vao, 1, VBO_BINDING_INDEX);

		// a_particle_size
		glEnableVertexArrayAttrib(vao, 2);
		glVertexArrayAttribFormat(
			vao,
			2,		// attrib idx
			2,		// size
			GL_FLOAT,
			false,
			offsetof(NIKE::SysParticle::Particle, size)
		);
		glVertexArrayAttribBinding(vao, 2, VBO_BINDING_INDEX);

		// a_particle_rotation
		glEnableVertexArrayAttrib(vao, 3);
		glVertexArrayAttribFormat(
			vao,
			3,		// attrib idx
			1,		// size
			GL_FLOAT,
			false,
			offsetof(NIKE::SysParticle::Particle, rotation)
		);
		glVertexArrayAttribBinding(vao, 3, VBO_BINDING_INDEX);

		// for instanced rendering
		glVertexArrayBindingDivisor(vao, VBO_BINDING_INDEX, 1);

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	} 

	void Assets::RenderLoader::createBatchedBaseBuffers(Model& model) {
		// only handles drwaing quads

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

		// create vao
		glCreateVertexArrays(1, &model.vaoid);

		// create vbo
		glCreateBuffers(1, &model.vboid);

		// create ebo
		glCreateBuffers(1, &model.eboid);

		// bind vbo and ebo to vao
		static constexpr int VBO_BINDING_INDEX = 10;
		static constexpr int VERTEX_SIZE = sizeof(Vertex);
		glVertexArrayVertexBuffer(model.vaoid, VBO_BINDING_INDEX, model.vboid, 0, VERTEX_SIZE);
		glVertexArrayElementBuffer(model.vaoid, model.eboid);


		// allocate space for vbo
		static constexpr int NUM_VERTEX_PER_INSTANCE = 4;
		static const int MAX_VBO_SIZE = NIKE_RENDER_SERVICE->MAX_INSTANCES * NUM_VERTEX_PER_INSTANCE * VERTEX_SIZE;
		glNamedBufferStorage(model.vboid, MAX_VBO_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// allocate space for ebo
		static constexpr int NUM_INDICES_PER_INSTANCE = 6;
		static constexpr int INDEX_SIZE = sizeof(unsigned int);
		static const int MAX_EBO_SIZE = NIKE_RENDER_SERVICE->MAX_INSTANCES * NUM_INDICES_PER_INSTANCE * INDEX_SIZE;
		glNamedBufferStorage(model.eboid, MAX_EBO_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// set vertex array attributes

		// batched_base.vert location=0
		static constexpr int POSITION_ATTRIB_INDEX = 0;
		static constexpr int POSITION_ATTRIB_SIZE = 2;		// num elements (x,y)
		static constexpr int POSITION_DATA_TYPE = GL_FLOAT;
		glEnableVertexArrayAttrib(model.vaoid, POSITION_ATTRIB_INDEX);		// vertex attrib index 0
		glVertexArrayAttribFormat(
			model.vaoid,
			POSITION_ATTRIB_INDEX,
			POSITION_ATTRIB_SIZE,
			POSITION_DATA_TYPE,
			false,		//normalized
			offsetof(Vertex, pos)			// offset
		);
		glVertexArrayAttribBinding(model.vaoid, POSITION_ATTRIB_INDEX, VBO_BINDING_INDEX);

		// batched_base.vert location=1
		static constexpr int COLOR_ATTRIB_INDEX = 1;
		static constexpr int COLOR_ATTRIB_SIZE = 4;		// num elements (r,g,b,a)
		static constexpr int COLOR_DATA_TYPE = GL_FLOAT;
		static constexpr int COLOR_DATA_OFFSET = offsetof(Vertex, col);
		glEnableVertexArrayAttrib(model.vaoid, COLOR_ATTRIB_INDEX);		// vertex attrib index 1
		glVertexArrayAttribFormat(
			model.vaoid,
			COLOR_ATTRIB_INDEX,
			COLOR_ATTRIB_SIZE,
			COLOR_DATA_TYPE,
			false,		//normalized
			COLOR_DATA_OFFSET		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, COLOR_ATTRIB_INDEX, VBO_BINDING_INDEX);

		// batched_base.vert location=4
		static constexpr int XFORM_ATTRIB_INDEX_0 = 4;
		static constexpr int XFORM_ATTRIB_SIZE = 3;		// num elements(row of 3x3 mtx)
		static constexpr int XFORM_DATA_TYPE = GL_FLOAT;
		static constexpr int XFORM_DATA_OFFSET_0 = offsetof(Vertex, transform);
		glEnableVertexArrayAttrib(model.vaoid, XFORM_ATTRIB_INDEX_0);		// vertex attrib index 4
		glVertexArrayAttribFormat(
			model.vaoid,
			XFORM_ATTRIB_INDEX_0,
			XFORM_ATTRIB_SIZE,
			XFORM_DATA_TYPE,
			false,		//normalized
			XFORM_DATA_OFFSET_0		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, XFORM_ATTRIB_INDEX_0, VBO_BINDING_INDEX);

		static constexpr int XFORM_ATTRIB_INDEX_1 = 5;
		static constexpr int XFORM_DATA_OFFSET_1 = XFORM_DATA_OFFSET_0 + sizeof(std::array<float, 3>);
		glEnableVertexArrayAttrib(model.vaoid, XFORM_ATTRIB_INDEX_1);		// vertex attrib index 5
		glVertexArrayAttribFormat(
			model.vaoid,
			XFORM_ATTRIB_INDEX_1,
			XFORM_ATTRIB_SIZE,
			XFORM_DATA_TYPE,
			false,		//normalized
			XFORM_DATA_OFFSET_1		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, XFORM_ATTRIB_INDEX_1, VBO_BINDING_INDEX);

		static constexpr int XFORM_ATTRIB_INDEX_2 = 6;
		static constexpr int XFORM_DATA_OFFSET_2 = XFORM_DATA_OFFSET_1 + sizeof(std::array<float, 3>);
		glEnableVertexArrayAttrib(model.vaoid, XFORM_ATTRIB_INDEX_2);		// vertex attrib index 6
		glVertexArrayAttribFormat(
			model.vaoid,
			XFORM_ATTRIB_INDEX_2,
			XFORM_ATTRIB_SIZE,
			XFORM_DATA_TYPE,
			false,		//normalized
			XFORM_DATA_OFFSET_2		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, XFORM_ATTRIB_INDEX_2, VBO_BINDING_INDEX);

		// vbo and ebo data population will be done in update

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}

	void Assets::RenderLoader::createBatchedTextureBuffers(Model& model) {

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

		while (glGetError() != GL_NO_ERROR) {}

		// !NOTE: n.loo
		//glFinish();
		//GLint maxVertexAttribs;

		//glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
		//if (13 >= maxVertexAttribs) {  // 13 might be too high
		//	NIKEE_CORE_ERROR("Attribute index {0} exceeds maximum supported attributes: {1}",
		//		13, maxVertexAttribs);
		//	return;
		//}

		//GLint previousVAO, previousVBO;
		//glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO);
		//glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousVBO);


		// create vao
		glCreateVertexArrays(1, &model.vaoid);

		// create vbo
		glCreateBuffers(1, &model.vboid);

		// create ebo
		glCreateBuffers(1, &model.eboid);

		// bind vbo and ebo to vao
		static constexpr int VBO_BINDING_INDEX = 10;
		static constexpr int VERTEX_SIZE = sizeof(Vertex);
		glVertexArrayVertexBuffer(model.vaoid, VBO_BINDING_INDEX, model.vboid, 0, VERTEX_SIZE);
		glVertexArrayElementBuffer(model.vaoid, model.eboid);


		// allocate space for vbo
		static constexpr int NUM_VERTEX_PER_INSTANCE = 4;
		static const int MAX_VBO_SIZE = NIKE_RENDER_SERVICE->MAX_INSTANCES * NUM_VERTEX_PER_INSTANCE * VERTEX_SIZE;
		glNamedBufferStorage(model.vboid, MAX_VBO_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// allocate space for ebo
		static constexpr int NUM_INDICES_PER_INSTANCE = 6;
		static constexpr int INDEX_SIZE = sizeof(unsigned int);
		static const int MAX_EBO_SIZE = NIKE_RENDER_SERVICE->MAX_INSTANCES * NUM_INDICES_PER_INSTANCE * INDEX_SIZE;
		glNamedBufferStorage(model.eboid, MAX_EBO_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// set vertex array attributes

		// batched_texture.vert location=0
		static constexpr int POSITION_ATTRIB_INDEX = 0;
		static constexpr int POSITION_ATTRIB_SIZE = 2;		// num elements (x,y)
		static constexpr int POSITION_DATA_TYPE = GL_FLOAT;
		glEnableVertexArrayAttrib(model.vaoid, POSITION_ATTRIB_INDEX);		// vertex attrib index 0
		glVertexArrayAttribFormat(
			model.vaoid,
			POSITION_ATTRIB_INDEX,
			POSITION_ATTRIB_SIZE,
			POSITION_DATA_TYPE,
			false,		//normalized
			offsetof(Vertex, pos)			// offset
		);
		glVertexArrayAttribBinding(model.vaoid, POSITION_ATTRIB_INDEX, VBO_BINDING_INDEX);

		// batched_texture.vert location=1
		static constexpr int TEXCOORD_ATTRIB_INDEX = 1;
		static constexpr int TEXCOORD_ATTRIB_SIZE = 2;		// num elements (x,y)
		static constexpr int TEXCOORD_DATA_TYPE = GL_FLOAT;
		static constexpr int TEXCOORD_DATA_OFFSET = offsetof(Vertex, tex_coords);
		glEnableVertexArrayAttrib(model.vaoid, TEXCOORD_ATTRIB_INDEX);		// vertex attrib index 1
		glVertexArrayAttribFormat(
			model.vaoid,
			TEXCOORD_ATTRIB_INDEX,
			TEXCOORD_ATTRIB_SIZE,
			TEXCOORD_DATA_TYPE,
			false,		//normalized
			TEXCOORD_DATA_OFFSET		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, TEXCOORD_ATTRIB_INDEX, VBO_BINDING_INDEX);

		// batched_texture.vert location=2
		static constexpr int SAMPLERIDX_ATTRIB_INDEX = 2;
		static constexpr int SAMPLERIDX_ATTRIB_SIZE = 1;		// num elements (index)
		static constexpr int SAMPLERIDX_DATA_TYPE = GL_FLOAT;
		static constexpr int SAMPLERIDX_DATA_OFFSET = offsetof(Vertex, sampler_idx);
		glEnableVertexArrayAttrib(model.vaoid, SAMPLERIDX_ATTRIB_INDEX);		// vertex attrib index 1
		glVertexArrayAttribFormat(
			model.vaoid,
			SAMPLERIDX_ATTRIB_INDEX,
			SAMPLERIDX_ATTRIB_SIZE,
			SAMPLERIDX_DATA_TYPE,
			false,		//normalized
			SAMPLERIDX_DATA_OFFSET		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, SAMPLERIDX_ATTRIB_INDEX, VBO_BINDING_INDEX);
		// glVertexAttribDivisor(SAMPLERIDX_ATTRIB_INDEX, 1);		// per instance only. do not interpolate(nvm useless)

		// batched_texture.vert location=4
		static constexpr int XFORM_ATTRIB_INDEX_0 = 4;
		static constexpr int XFORM_ATTRIB_SIZE = 3;		// num elements(row of 3x3 mtx)
		static constexpr int XFORM_DATA_TYPE = GL_FLOAT;
		static constexpr int XFORM_DATA_OFFSET_0 = offsetof(Vertex, transform);
		glEnableVertexArrayAttrib(model.vaoid, XFORM_ATTRIB_INDEX_0);		// vertex attrib index 4
		glVertexArrayAttribFormat(
			model.vaoid,
			XFORM_ATTRIB_INDEX_0,
			XFORM_ATTRIB_SIZE,
			XFORM_DATA_TYPE,
			false,		//normalized
			XFORM_DATA_OFFSET_0		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, XFORM_ATTRIB_INDEX_0, VBO_BINDING_INDEX);

		static constexpr int XFORM_ATTRIB_INDEX_1 = 5;
		static constexpr int XFORM_DATA_OFFSET_1 = XFORM_DATA_OFFSET_0 + sizeof(std::array<float, 3>);
		glEnableVertexArrayAttrib(model.vaoid, XFORM_ATTRIB_INDEX_1);		// vertex attrib index 5
		glVertexArrayAttribFormat(
			model.vaoid,
			XFORM_ATTRIB_INDEX_1,
			XFORM_ATTRIB_SIZE,
			XFORM_DATA_TYPE,
			false,		//normalized
			XFORM_DATA_OFFSET_1		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, XFORM_ATTRIB_INDEX_1, VBO_BINDING_INDEX);

		static constexpr int XFORM_ATTRIB_INDEX_2 = 6;
		static constexpr int XFORM_DATA_OFFSET_2 = XFORM_DATA_OFFSET_1 + sizeof(std::array<float, 3>);
		glEnableVertexArrayAttrib(model.vaoid, XFORM_ATTRIB_INDEX_2);		// vertex attrib index 6
		glVertexArrayAttribFormat(
			model.vaoid,
			XFORM_ATTRIB_INDEX_2,
			XFORM_ATTRIB_SIZE,
			XFORM_DATA_TYPE,
			false,		//normalized
			XFORM_DATA_OFFSET_2		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, XFORM_ATTRIB_INDEX_2, VBO_BINDING_INDEX);

		// batched_texture.vert location=8
		static constexpr int FRAMESIZE_ATTRIB_INDEX = 8;
		static constexpr int FRAMESIZE_ATTRIB_SIZE = 2;		// num elements (x, y)
		static constexpr int FRAMESIZE_DATA_TYPE = GL_FLOAT;
		static constexpr int FRAMESIZE_DATA_OFFSET = offsetof(Vertex, framesize);
		glEnableVertexArrayAttrib(model.vaoid, FRAMESIZE_ATTRIB_INDEX);		// vertex attrib index 1
		glVertexArrayAttribFormat(
			model.vaoid,
			FRAMESIZE_ATTRIB_INDEX,
			FRAMESIZE_ATTRIB_SIZE,
			FRAMESIZE_DATA_TYPE,
			false,		//normalized
			FRAMESIZE_DATA_OFFSET		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, FRAMESIZE_ATTRIB_INDEX, VBO_BINDING_INDEX);

		// batched_texture.vert location=9
		static constexpr int UVOFFSET_ATTRIB_INDEX = 9;
		static constexpr int UVOFFSET_ATTRIB_SIZE = 2;		// num elements (x, y)
		static constexpr int UVOFFSET_DATA_TYPE = GL_FLOAT;
		static constexpr int UVOFFSET_DATA_OFFSET = offsetof(Vertex, uv_offset);
		glEnableVertexArrayAttrib(model.vaoid, UVOFFSET_ATTRIB_INDEX);		// vertex attrib index 1
		glVertexArrayAttribFormat(
			model.vaoid,
			UVOFFSET_ATTRIB_INDEX,
			UVOFFSET_ATTRIB_SIZE,
			UVOFFSET_DATA_TYPE,
			false,		//normalized
			UVOFFSET_DATA_OFFSET		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, UVOFFSET_ATTRIB_INDEX, VBO_BINDING_INDEX);

		// batched_texture.vert location=11
		static constexpr int BLEND_ATTRIB_INDEX = 11;
		static constexpr int BLEND_ATTRIB_SIZE = 1;		// num elements (x, y)
		static constexpr int BLEND_DATA_TYPE = GL_FLOAT;
		static constexpr int BLEND_DATA_OFFSET = offsetof(Vertex, to_blend_color);
		glEnableVertexArrayAttrib(model.vaoid, BLEND_ATTRIB_INDEX);		// vertex attrib index 1
		glVertexArrayAttribFormat(
			model.vaoid,
			BLEND_ATTRIB_INDEX,
			BLEND_ATTRIB_SIZE,
			BLEND_DATA_TYPE,
			false,		//normalized
			BLEND_DATA_OFFSET		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, BLEND_ATTRIB_INDEX, VBO_BINDING_INDEX);

		// batched_texture.vert location=12
		static constexpr int BLENDCOLOR_ATTRIB_INDEX = 12;
		static constexpr int BLENDCOLOR_ATTRIB_SIZE = 4;		// num elements (x, y)
		static constexpr int BLENDCOLOR_DATA_TYPE = GL_FLOAT;
		static constexpr int BLENDCOLOR_DATA_OFFSET = offsetof(Vertex, col);
		glEnableVertexArrayAttrib(model.vaoid, BLENDCOLOR_ATTRIB_INDEX);		// vertex attrib index 1
		glVertexArrayAttribFormat(
			model.vaoid,
			BLENDCOLOR_ATTRIB_INDEX,
			BLENDCOLOR_ATTRIB_SIZE,
			BLENDCOLOR_DATA_TYPE,
			false,		//normalized
			BLENDCOLOR_DATA_OFFSET		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, BLENDCOLOR_ATTRIB_INDEX, VBO_BINDING_INDEX);

		// batched_texture.vert location=13
		static constexpr int BLENDINTENSITY_ATTRIB_INDEX = 13;
		static constexpr int BLENDINTENSITY_ATTRIB_SIZE = 1;		// num elements (x, y)
		static constexpr int BLENDINTENSITY_DATA_TYPE = GL_FLOAT;
		static constexpr int BLENDINTENSITY_DATA_OFFSET = offsetof(Vertex, blend_intensity);
		glEnableVertexArrayAttrib(model.vaoid, BLENDINTENSITY_ATTRIB_INDEX);		// vertex attrib index 1
		glVertexArrayAttribFormat(
			model.vaoid,
			BLENDINTENSITY_ATTRIB_INDEX,
			BLENDINTENSITY_ATTRIB_SIZE,
			BLENDINTENSITY_DATA_TYPE,
			false,		//normalized
			BLENDINTENSITY_DATA_OFFSET		// offset
		);
		glVertexArrayAttribBinding(model.vaoid, BLENDINTENSITY_ATTRIB_INDEX, VBO_BINDING_INDEX);

		// vbo and ebo data population will be done in update

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}

		// !NOTE: n.loo
		//glBindVertexArray(previousVAO);
		//glBindBuffer(GL_ARRAY_BUFFER, previousVBO);
	}

	void Assets::RenderLoader::createTextureBuffers(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices, const std::vector<Vector2f>& tex_coords, Assets::Model& model) {
		// VBO (Vertex Buffer Object)
		glCreateBuffers(1, &model.vboid);
		glNamedBufferStorage(model.vboid,
			sizeof(Vertex) * vertices.size(),
			nullptr, // nullptr means no data is transferred
			GL_DYNAMIC_STORAGE_BIT);

		std::vector<Vertex> rendering_vertices;
		rendering_vertices.reserve(vertices.size());
		for (size_t i = 0; i < vertices.size(); ++i) {
			Vertex v;
			v.pos = vertices[i];
			v.tex_coords = tex_coords[i];
			rendering_vertices.push_back(v);
		}

		glNamedBufferSubData(model.vboid, 0, sizeof(Vertex) * vertices.size(), rendering_vertices.data());

		// VAO (Vertex Array Object)
		glCreateVertexArrays(1, &model.vaoid);

		static constexpr int BINDING_INDEX = 0;
		glVertexArrayVertexBuffer(model.vaoid, BINDING_INDEX, model.vboid, 0, sizeof(Vertex));

		// Vertex Position Array
		glEnableVertexArrayAttrib(model.vaoid, 0); // vertex attribute index 0
		glVertexArrayAttribFormat(model.vaoid, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
		glVertexArrayAttribBinding(model.vaoid, 0, BINDING_INDEX);

		// Vertex texture coordinates
		glEnableVertexArrayAttrib(model.vaoid, 2);
		glVertexArrayAttribFormat(model.vaoid, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, tex_coords));
		glVertexArrayAttribBinding(model.vaoid, 2, BINDING_INDEX);

		// Create EBO
		glCreateBuffers(1, &model.eboid);
		glNamedBufferStorage(model.eboid, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayElementBuffer(model.vaoid, model.eboid);
	}

	unsigned char* Assets::RenderLoader::prepareImageData(const std::string& path_to_texture, int& width, int& height, int& tex_size, bool& is_tex_or_png_ext) {
		// find file type
		std::string filetype = path_to_texture.substr(path_to_texture.find_last_of('.') + 1);
		const std::set<std::string> valid_tex_ext = { "png", "jpg", "jpeg", "ico" }; // accepted file types

		// Transform each character in string ext to lowercase
		for (char& c : filetype) {
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}

		/*
		if (filetype == "tex") {
			is_tex_or_png_ext = true;

			width = 256;
			height = 256;

			std::ifstream texture_file{ path_to_texture, std::ios::binary | std::ios::ate };
			if (!texture_file.is_open()) {
				NIKEE_CORE_ERROR("Failed to open texture file: {}", path_to_texture);
				return nullptr;
			}

			// get tex_size of texture file
			tex_size = static_cast<int>(texture_file.tellg());
			if (tex_size <= 0) {
				NIKEE_CORE_ERROR("Texture file is empty or invalid: {}", path_to_texture);
				return nullptr;
			}
			// return to beginning of file
			texture_file.seekg(0, std::ios::beg);

			char* tex_data{ new char[tex_size] };

			// read tex data into ptr
			if (!texture_file.read(reinterpret_cast<char*>(tex_data), tex_size)) {
				NIKEE_CORE_ERROR("Failed to read texture file: {}", path_to_texture);
				delete[] tex_data;  // Free memory before returning
				return nullptr;
			}
			texture_file.close();

			return tex_data;
		}
		*/

		// is not .tex file
		if (valid_tex_ext.find(filetype) != valid_tex_ext.end()) {
			is_tex_or_png_ext = true;
			int channels;
			stbi_set_flip_vertically_on_load(true);
			static constexpr int desired_channels = 4;
			unsigned char* img_data = stbi_load(path_to_texture.c_str(), &width, &height, &channels, desired_channels);
			if (img_data == nullptr || width == 0 || height == 0) {
				NIKEE_CORE_ERROR("Failed to load image data: {}", path_to_texture);
				NIKEE_CORE_ERROR("stb_image error:  {}", stbi_failure_reason());
				stbi_image_free(img_data); // Free memory
				return nullptr; // Return nullptr if loading fails
			}

			// Reject if the image is larger than 4K 
			if (width > 4096 || height > 4096) {
				NIKEE_CORE_ERROR("Image is larger than 4K resolution: {} ({}x{})", path_to_texture, width, height);
				stbi_image_free(img_data); // Free memory
				return nullptr; // Return nullptr if image is too large
			}

			tex_size = width * height * 4;  // RGBA format

			// flip image around for opengl
			/*
			const int row_size = width * channels;
			for (int i = 0; i < height / 2; ++i) {
				unsigned char* topRow = img_data + i * row_size;
				unsigned char* bottomRow = img_data + (height - 1 - i) * row_size;

				for (int j = 0; j < row_size; ++j) {
					std::swap(topRow[j], bottomRow[j]);
				}
			}
			*/

			return img_data;
		}

		// If the file type is unsupported (not .tex, .png, .jpg, or .jpeg)
		is_tex_or_png_ext = false;
		NIKEE_CORE_ERROR("Unsupported file format: {}", path_to_texture);
		//glFinish();
		return nullptr;

	}

	void Assets::RenderLoader::freeImageData(unsigned char* img_data) {
		stbi_image_free(img_data);
	}

	Assets::Model Assets::RenderLoader::compileModel(const std::string& path_to_mesh) {
		Assets::Model model;

		std::ifstream mesh_file{ path_to_mesh, std::ios::in };
		if (!mesh_file.is_open()) {
			throw std::runtime_error("Failed to open model file.");
		}

		mesh_file.seekg(0, std::ios::beg);

		std::string line;
		GLshort index;

		// pos
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
				Vertex v;
				v.pos = { ndc_x, ndc_y };
				model.vertices.emplace_back(v);
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
				throw std::runtime_error("Unknown data type in mesh file: " + std::string(1, type));
			}
		}

		std::vector<Vector2f> pos_vertices;
		pos_vertices.reserve(model.vertices.size());
		for (const Vertex& v : model.vertices) {
			pos_vertices.emplace_back(v.pos);
		}

		if (tex_coords.size() && tex_coords.size() != pos_vertices.size()) {
			throw std::runtime_error("Texture coordinates do not match number of vertices.");
		}

		// set texcoords into model vertex
		for (int i{}; i < tex_coords.size(); i++) {
			model.vertices[i].tex_coords = tex_coords[i];
		}

		static constexpr const char* BATCHED_PREFIX = "batched_";
		const bool for_batched_rendering = path_to_mesh.find(BATCHED_PREFIX) != std::string::npos;

		if (tex_coords.size() == 0) {
			if (for_batched_rendering) {
				createBatchedBaseBuffers(model);
			}
			else {
				createBaseBuffers(pos_vertices, indices, model);
			}
		}
		else {
			if (for_batched_rendering) {
				createBatchedTextureBuffers(model);
			}
			else {
				createTextureBuffers(pos_vertices, indices, tex_coords, model);
			}
		}
		model.draw_count = static_cast<GLuint>(indices.size());
		model.indices = indices;

		NIKEE_CORE_INFO("Sucessfully loaded model from " + path_to_mesh);

		return model;
	}

	Assets::Texture Assets::RenderLoader::compileTexture(const std::string& path_to_texture) {
		// find file type
		std::string filetype = path_to_texture.substr(path_to_texture.find_last_of('.') + 1);

		int tex_width{};
		int tex_height{};
		int tex_size{};
		bool is_tex_or_png_ext = false;
		const unsigned char* tex_data = prepareImageData(path_to_texture, tex_width, tex_height, tex_size, is_tex_or_png_ext);

		if (tex_data == nullptr) {
			NIKEE_CORE_ERROR("Failed to load image : {} ", path_to_texture);
			int invalid = -1;
			int def_size = 256;
			return Assets::Texture(static_cast<unsigned>(invalid), { def_size, def_size }, path_to_texture);
		}

		// create texture
		unsigned int tex_id;
		glCreateTextures(GL_TEXTURE_2D, 1, &tex_id);
		glTextureStorage2D(tex_id, 1, GL_RGBA8, tex_width, tex_height);
		glTextureSubImage2D(tex_id, 0, 0, 0, tex_width, tex_height, (is_tex_or_png_ext ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, tex_data);

		stbi_image_free(static_cast<unsigned char*>(const_cast<void*>(reinterpret_cast<const void*>(tex_data))));

		NIKEE_CORE_INFO("Sucessfully loaded texture from " + path_to_texture);

		// Return texture
		return Assets::Texture(tex_id, { tex_width, tex_height }, path_to_texture);
	}
}

#undef STB_IMAGE_IMPLEMENTATION
