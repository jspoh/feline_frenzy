/*****************************************************************//**
 * \file   RenderManager.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/


#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysRender.h"
#include "../headers/Core/Engine.h"

void RenderManager::compileShader(std::string shader_ref, std::string vtx_path, std::string frag_path) {
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

	shader_programs[shader_ref] = shader_handle;
}

RenderManager::RenderManager() {
	// compile shaders
	compileShader("base", "shaders/base.vert", "shaders/base.frag");
	compileShader("tex", "shaders/textured_rendering.vert", "shaders/textured_rendering.frag");
	// !TODO: shader for text rendering(?)
}

RenderManager::~RenderManager() {
	for (auto& [ref, model] : models) {
		glDeleteVertexArrays(1, &model.vaoid);
	}
}

RenderManager& RenderManager::getInstance() {
	static RenderManager instance;
	return instance;
}

// only for base rendering shader because of attrib locations
bool RenderManager::registerMesh(std::string mesh_ref, std::string path_to_mesh) {
	std::ifstream mesh_file{ path_to_mesh };
	if (!mesh_file.is_open()) {
		cerr << "Failed to open mesh file: " << path_to_mesh << endl;
		return false;
	}

	// placeholder for parsing
	char placeholder;

	int vertex_count, index_count;
	mesh_file >> vertex_count >> placeholder >> index_count;

	// pos
	std::vector<Vector2> vertices;
	vertices.reserve(vertex_count);

	// color
	std::vector<Vector4> colors;
	colors.reserve(vertex_count);

	// indices (indexed rendering with element buffer object)
	std::vector<unsigned int> indices;
	indices.reserve(index_count);

	// line data type (eg. vertex, color, indices)
	char type;

	while (mesh_file >> type) {
		switch (type) {
		case 'v': {
			float ndc_x, ndc_y;
			mesh_file
				>> ndc_x >> placeholder
				>> ndc_y;
			vertices.emplace_back(ndc_x, ndc_y);
			break;
		}
		case 'c': {
			float r, g, b, a;
			mesh_file
				>> r >> placeholder
				>> g >> placeholder
				>> b >> placeholder
				>> a;
			colors.emplace_back(r, g, b, a);
			break;
		}
		case 'i': {
			for (int _{}; _ < index_count; _++) {
				unsigned int i;
				mesh_file
					>> i >> placeholder;
				indices.push_back(i);
			}
			break;
		}
		default:
			cerr << "Execution should not have reached here. type: " << type << endl;
			break;
		}
	}

	// ensure that the vertex and index count matches the file
	if (vertices.size() != vertex_count) {
		cerr << "Vertex count mismatch. Expected: " << vertex_count << " Actual: " << vertices.size() << endl;
		throw std::exception();
	}
	if (indices.size() != index_count) {
		cerr << "Index count mismatch. Expected: " << index_count << " Actual: " << indices.size() << endl;
		throw std::exception();
	}

	
	// calculate offset and sizes
	const int pos_data_offset = 0;
	const int pos_attr_size = sizeof(Vector2);
	const int pos_data_size = vertex_count * pos_attr_size;

	const int col_data_offset = pos_data_size;
	const int col_attr_size = sizeof(Vector4);
	const int col_data_size = vertex_count * col_attr_size;

	const int idx_attr_size = sizeof(unsigned int);
	const int idx_data_size = index_count * idx_attr_size;

	// create vertex buffer object and populate buffer
	unsigned int vbo;
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, pos_data_size + col_data_size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(vbo, pos_data_offset, pos_data_size, vertices.data());
	glNamedBufferSubData(vbo, col_data_offset, col_data_size, colors.data());

	// create vertex array object
	unsigned int vao;
	glCreateVertexArrays(1, &vao);

	// vertex shader attributes
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayVertexBuffer(vao, 3, vbo, pos_data_offset, pos_attr_size);
	glVertexArrayAttribFormat(
		vao,
		0,		// attrib idx(location)
		2,		// num coords (x,y)
		GL_FLOAT,	// data type
		false,		// normalized
		0		// relative offset
	);
	glVertexArrayAttribBinding(vao, 0, 3);

	// fragment shader attributes
	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayVertexBuffer(vao, 4, vbo, col_data_offset, col_attr_size);
	glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, false, 0);
	glVertexArrayAttribBinding(vao, 1, 4);

	// element buffer object for indexed rendering
	unsigned int ebo;
	glCreateBuffers(1, &ebo);
	glNamedBufferStorage(ebo, idx_data_size, indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vao, ebo);

	// create model
	Model model;
	model.vaoid = vao;
	model.primitive_type = GL_TRIANGLES;
	model.draw_count = index_count;

	models[mesh_ref] = model;

	return true;
}

void RenderManager::useShader(std::string shader_ref) {
	if (shader_programs.find(shader_ref) == shader_programs.end()) {
		cerr << "Shader reference not found: " << shader_ref << endl;
		throw std::exception();
	}

	unsigned int shader_program = shader_programs[shader_ref];
	glUseProgram(shader_program);
}

void RenderManager::unuseShader() {
	glUseProgram(0);
}

void RenderManager::drawModel(std::string mdl_ref) {
	const auto& model = models.at(mdl_ref);

	useShader("base");
	glBindVertexArray(model.vaoid);
	
	glDrawElements(model.primitive_type, model.draw_count, GL_UNSIGNED_INT, nullptr);
	
	glBindVertexArray(0);
	unuseShader();
}
