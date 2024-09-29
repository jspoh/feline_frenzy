/*****************************************************************//**
 * \file   RenderManager.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Components/cTransform.h"
#include "../headers/Components/cRender.h"
#include "../headers/Math/Mtx33.h"

Render::Manager::~Manager() {

	//Clear models
	for (auto& model : models) {
		glDeleteVertexArrays(1, &model.second->vaoid);
		glDeleteBuffers(1, &model.second->vboid);
		glDeleteBuffers(1, &model.second->eboid);
	}
}

void Render::Manager::createBuffers(const std::vector<Vector2>& vertices, const std::vector<unsigned int>& indices, std::shared_ptr<Model> model) {
	// VBO (Vertex Buffer Object)
	glCreateBuffers(1, &model->vboid);
	glNamedBufferStorage(model->vboid,
		sizeof(Vector2) * vertices.size(),
		nullptr, // nullptr means no data is transferred
		GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(model->vboid, 0, sizeof(Vector2) * vertices.size(), vertices.data());

	// VAO (Vertex Array Object)
	glCreateVertexArrays(1, &model->vaoid);

	// Vertex Position Array
	glEnableVertexArrayAttrib(model->vaoid, 0); // vertex attribute index 0
	glVertexArrayVertexBuffer(model->vaoid, 0, model->vboid, 0, sizeof(Vector2)); // buffer binding point 0
	glVertexArrayAttribFormat(model->vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(model->vaoid, 0, 0);

	// REMOVED Vertex Color Array (Using uniform to pass color)

	// Create EBO
	glCreateBuffers(1, &model->eboid);
	glNamedBufferStorage(model->eboid, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(model->vaoid, model->eboid);
}

bool Render::Manager::loadMesh(const std::string& path_to_mesh, std::shared_ptr<Model> model) {
	std::ifstream mesh_file{ path_to_mesh, std::ios::in };
	if (!mesh_file.is_open()) {
		cerr << "Failed to open mesh file: " << path_to_mesh << endl;
		return false;
	}

	mesh_file.seekg(0, std::ios::beg);

	std::string line;
	float ndc_x, ndc_y;
	GLshort index;

	// pos
	std::vector<Vector2> vertices;

	// indices (indexed rendering with element buffer object)
	std::vector<unsigned int> indices;


	// line data type (eg. vertex, color, indices)
	char type;

	while (getline(mesh_file, line)){
		std::istringstream line_sstm{ line };

		line_sstm >> type;

		switch (type) {
		case 'n': {
			break;
		}
		case 'v': {

			line_sstm >> ndc_x >> ndc_y;
			vertices.emplace_back(ndc_x, ndc_y);
			break;
		}
		case 't': {
			if (model->primitive_type == 0) {
				model->primitive_type = GL_TRIANGLES;
			}
			while (line_sstm >> index) { // Grab index position
				indices.emplace_back(index);
			}
			break;
		}

		case 'f': {
			if (model->primitive_type == 0) {
				model->primitive_type = GL_TRIANGLE_FAN;
			}
			while (line_sstm >> index) { // Grab index position
				indices.emplace_back(index);
			}
			break;
		}
		default:
			cerr << "Unknown data type in mesh file: " << type << endl;
			return false;
		}
	}
	mesh_file.close();

	createBuffers(vertices, indices, model);
	model->draw_count = static_cast<GLuint>(indices.size());

	return true;
}

void Render::Manager::transformMatrix(Transform::Transform& xform, Render::Mesh& mesh, Matrix33::Matrix_33 world_to_ndc_mat) {
	//Transform matrix here
	Matrix33::Matrix_33 model_mat, result, scale_mat, rot_mat, trans_mat;
	float orientation = 0.0f;

	// Modulus the object rotation so it doesnt result in a large number overtime
	orientation = fmod(orientation, 360.f);
	float angleDisp = (orientation += xform.rotation * NIKEEngine.getDeltaTime()) * static_cast<float>(M_PI) / 180.f;

	Matrix_33Rot(rot_mat, angleDisp);
	Matrix_33Scale(scale_mat, xform.scale.x, xform.scale.y);
	Matrix_33Translate(trans_mat, xform.position.x, xform.position.y);
	result = world_to_ndc_mat * trans_mat * rot_mat * scale_mat;

	// OpenGL requires matrix in col maj so transpose
	Matrix_33Transpose(mesh.x_form, result);
}

void Render::Manager::renderObject(Render::Mesh const& e_mesh, Render::Color const& e_color) {
	//Set Polygon Mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Use shader
	shader_system->useShader(e_mesh.shader_ref);

	//Shader set uniform
	shader_system->setUniform(e_mesh.shader_ref, "f_color", e_color.color);
	shader_system->setUniform(e_mesh.shader_ref, "model_to_ndc", e_mesh.x_form);

	//Find model
	assert(models.find(e_mesh.model_ref) != models.end() && "Model not found.");
	auto& model = models.at(e_mesh.model_ref);

	//Draw model
	glBindVertexArray(model->vaoid);
	glDrawElements(model->primitive_type, model->draw_count, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	//Unuse shader
	shader_system->unuseShader();
}

void Render::Manager::renderWireFrame(Render::Mesh const& e_mesh, Render::Color const& e_color) {
	//Set Polygon Mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Use shader
	shader_system->useShader(e_mesh.shader_ref);

	//Shader set uniform
	//Scale wire frame
	Matrix33::Matrix_33 scaled_wire_frame = 1.01f * e_mesh.x_form;
	Matrix_33Transpose(scaled_wire_frame, scaled_wire_frame);
	shader_system->setUniform(e_mesh.shader_ref, "f_color", e_color.color);
	shader_system->setUniform(e_mesh.shader_ref, "model_to_ndc", scaled_wire_frame);

	//Find model
	assert(models.find(e_mesh.model_ref) != models.end() && "Model not found.");
	auto& model = models.at(e_mesh.model_ref);

	//Draw model
	glBindVertexArray(model->vaoid);
	glDrawElements(GL_TRIANGLES, model->draw_count, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	//Unuse shader
	shader_system->unuseShader();

	//Reset Polygon Mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Render::Manager::registerModel(const std::string& model_ref, const std::string& path_to_mesh) {
	std::shared_ptr model = std::make_shared<Model>();
	if (loadMesh(path_to_mesh, model)) {
		models[model_ref] = model;
	}
}

void Render::Manager::trackCamEntity(std::string const& cam_identifier) {
	camera_system->trackCamEntity(cam_identifier);
}

void Render::Manager::init() {
	//Create shader system
	shader_system = std::make_unique<Shader::Manager>();

	//Load shaders
	shader_system->loadShader("base", "shaders/base.vert", "shaders/base.frag");
	shader_system->loadShader("tex", "shaders/textured_rendering.vert", "shaders/textured_rendering.frag");

	//Create camera system
	camera_system = std::make_unique<Camera::System>();

	//Init Camera ( Camera height defaulted at 1000.0f )
	camera_system->init(1000.0f);
}

void Render::Manager::update() {
	
	// Before drawing clear screen
	glClear(GL_COLOR_BUFFER_BIT);

	//Update set of cameras
	std::set<Entity::Type> cam_entities;
	for (auto& entity : entities) {
		//Update camera position
		if (!NIKEEngine.checkEntityComponent<Render::Cam>(entity))
			continue;

		//Update camera and push to list
		auto& e_transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);
		auto& e_camera = NIKEEngine.getEntityComponent<Render::Cam>(entity);
		e_camera.position = e_transform.position;
		cam_entities.insert(entity);
	}

	//Update camera with set of cam entities
	camera_system->updateCameraEntities(std::move(cam_entities));

	//Update camera system
	camera_system->update();

	//Update all entities
	for (auto& entity : entities) {
		auto& e_transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);
		auto& e_mesh = NIKEEngine.getEntityComponent<Render::Mesh>(entity);
		auto& e_color = NIKEEngine.getEntityComponent<Render::Color>(entity);

		//Transform matrix here
		transformMatrix(e_transform, e_mesh, camera_system->getWorldToNDCXform());

		//Render object
		renderObject(e_mesh, e_color);

		//Render debugging wireframe
		Render::Color wire_frame_color{ { 1.0f, 0.0f, 0.0f }, 1.0f };
		renderWireFrame(e_mesh, wire_frame_color);
	}
}
