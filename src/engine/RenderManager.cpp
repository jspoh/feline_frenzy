/*****************************************************************//**
 * \file   RenderManager.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/


#include "stdafx.h"
#include "RenderManager.h"
#include "GLManager.h"


RenderManager::RenderManager() {
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


void RenderManager::drawModel(std::string mdl_ref) {
	const auto& model = models.at(mdl_ref);

	GLManager::getInstance().useShader("base");
	glBindVertexArray(model.vaoid);
	
	glDrawElements(model.primitive_type, model.draw_count, GL_UNSIGNED_INT, nullptr);
	
	glBindVertexArray(0);
	GLManager::getInstance().unuseShader();
}
