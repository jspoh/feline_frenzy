/*****************************************************************//**
 * \file   Model.cpp
 * \brief  Implementation of the Model class.
 *
 * \author jings
 * \date   September 2024
 *********************************************************************/
#include "stdafx.h"
#include "Model.h"

Model::Model() : vaoid(0), vboid(0), eboid(0), draw_count(0) {}

Model::~Model() {
    glDeleteVertexArrays(1, &vaoid);
    glDeleteBuffers(1, &vboid);
    glDeleteBuffers(1, &eboid);
}

// only for base rendering shader because of attrib locations
bool Model::loadMesh(const std::string& path_to_mesh) {
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
			cerr << "Unknown data type in mesh file: " << type << endl;
			return false;
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

	createBuffers(vertices, colors, indices);
	draw_count = index_count;

	return true;
}

void Model::createBuffers(const std::vector<Vector2>& vertices, const std::vector<Vector4>& colors, const std::vector<unsigned int>& indices) {
	// VBO (Vertex Buffer Object)
	glCreateBuffers(1, &vboid);
	glNamedBufferStorage(vboid,
		sizeof(Vector2) * vertices.size() + sizeof(Vector4) * colors.size(),
		nullptr, // nullptr means no data is transferred
		GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(vboid, 0, sizeof(Vector2) * vertices.size(), vertices.data());
	glNamedBufferSubData(vboid, vertices.size() * sizeof(Vector2), colors.size() * sizeof(Vector4), colors.data());

	// VAO (Vertex Array Object)
	glCreateVertexArrays(1, &vaoid);

	// Vertex Position Array
	glEnableVertexArrayAttrib(vaoid, 0); // vertex attribute index 0
	glVertexArrayVertexBuffer(vaoid, 0, vboid, 0, sizeof(Vector2)); // buffer binding point 0
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	// Vertex Color Array
	glEnableVertexArrayAttrib(vaoid, 1); // vertex attribute index 1
	glVertexArrayVertexBuffer(vaoid, 1, vboid, sizeof(Vector2) * vertices.size(), sizeof(Vector4)); // buffer binding point 1
	glVertexArrayAttribFormat(vaoid, 1, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 1, 1);

	// Create EBO
	glCreateBuffers(1, &eboid);
	glNamedBufferStorage(eboid, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, eboid);
}

void Model::draw() const {
	glBindVertexArray(vaoid);
	glDrawElements(GL_TRIANGLES, draw_count, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}