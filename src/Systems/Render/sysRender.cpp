/*****************************************************************//**
 * \file   sysRender.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \co-author Sean Gwee, g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Core/Engine.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Components/cTransform.h"
#include "../headers/Components/cRender.h"
#include "../headers/Math/Mtx33.h"

#define STB_IMAGE_IMPLEMENTATION
#include "data/stb_image.h"

void Render::Manager::createBaseBuffers(const std::vector<Vector2>& vertices, const std::vector<unsigned int>& indices, std::shared_ptr<Model> model) {
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

	// Create EBO
	glCreateBuffers(1, &model->eboid);
	glNamedBufferStorage(model->eboid, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(model->vaoid, model->eboid);
}

void Render::Manager::createTextureBuffers(const std::vector<Vector2>& vertices, const std::vector<unsigned int>& indices, const std::vector<Vector2>& tex_coords, std::shared_ptr<Render::Model> model) {
	// VBO (Vertex Buffer Object)
	glCreateBuffers(1, &model->vboid);
	glNamedBufferStorage(model->vboid,
		sizeof(Vector2) * vertices.size() + sizeof(Vector2) * tex_coords.size(),
		nullptr, // nullptr means no data is transferred
		GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(model->vboid, 0, sizeof(Vector2) * vertices.size(), vertices.data());
	glNamedBufferSubData(model->vboid, sizeof(Vector2) * vertices.size(), sizeof(Vector2) * tex_coords.size(), tex_coords.data());

	// VAO (Vertex Array Object)
	glCreateVertexArrays(1, &model->vaoid);

	// Vertex Position Array
	glEnableVertexArrayAttrib(model->vaoid, 0); // vertex attribute index 0
	glVertexArrayVertexBuffer(model->vaoid, 0, model->vboid, 0, sizeof(Vector2)); // buffer binding point 0
	glVertexArrayAttribFormat(model->vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(model->vaoid, 0, 0);

	// Vertex texture coordinates
	glEnableVertexArrayAttrib(model->vaoid, 2);
	glVertexArrayVertexBuffer(model->vaoid, 1, model->vboid, sizeof(Vector2) * vertices.size(), sizeof(Vector2));
	glVertexArrayAttribFormat(model->vaoid, 2, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(model->vaoid, 2, 1);

	// Create EBO
	glCreateBuffers(1, &model->eboid);
	glNamedBufferStorage(model->eboid, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(model->vaoid, model->eboid);
}

char* Render::Manager::prepareImageData(const std::string& path_to_texture, int& width, int& height, int& tex_size, bool& is_tex_or_png_ext) {
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

std::unordered_map<unsigned char, Render::Character> Render::Manager::registerFont(std::string const& file_path, Vector2 const& pixel_sizes) {
	return std::move(font_system->loadFont(file_path, pixel_sizes));
}

unsigned int Render::Manager::registerTexture(const std::string& path_to_texture) {
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

unsigned int Render::Manager::registerShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path) {
	return shader_system->loadShader(shader_ref, vtx_path, frag_path);
}

bool Render::Manager::loadMesh(const std::string& path_to_mesh, std::shared_ptr<Model> model) {
	std::ifstream mesh_file{ path_to_mesh, std::ios::in };
	if (!mesh_file.is_open()) {
		cerr << "Failed to open mesh file: " << path_to_mesh << endl;
		return false;
	}

	mesh_file.seekg(0, std::ios::beg);

	std::string line;
	GLshort index;

	// pos
	std::vector<Vector2> vertices;
	std::vector<Vector2> tex_coords;

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
			if (model->primitive_type == 0) {
				model->primitive_type = GL_TRIANGLES;
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

	if (tex_coords.size() == 0) {
		createBaseBuffers(vertices, indices, model);
	}
	else {
		createTextureBuffers(vertices, indices, tex_coords, model);
	}
	model->draw_count = static_cast<GLuint>(indices.size());

	return true;
}

std::shared_ptr<Render::Model> Render::Manager::registerModel(const std::string& path_to_mesh) {
	std::shared_ptr<Model> model = std::make_shared<Model>();
	if (loadMesh(path_to_mesh, model)) {
		return model;
	}

	return nullptr;
}

void Render::Manager::transformMatrix(Transform::Transform const& obj, Matrix33::Matrix_33& x_form, Matrix33::Matrix_33 world_to_ndc_mat) {
	//Transform matrix here
	Matrix33::Matrix_33 result, scale_mat, rot_mat, trans_mat;

	// Scrap orientation for now
	// Modulus the object rotation so it doesnt result in a large number overtime
	// float orientation = fmod(orientation, 360.f);
	//float angleDisp = (orientation += xform.rotation * NIKEEngine.getDeltaTime()) * static_cast<float>(M_PI) / 180.f;
	float angleDisp = obj.rotation;

	Matrix_33Rot(rot_mat, angleDisp);
	Matrix_33Scale(scale_mat, obj.scale.x, obj.scale.y);
	Matrix_33Translate(trans_mat, obj.position.x, obj.position.y);
	result = world_to_ndc_mat * trans_mat * rot_mat * scale_mat;

	// OpenGL requires matrix in col maj so transpose
	Matrix_33Transpose(x_form, result);
}

void Render::Manager::transformMatrixDebug(Transform::Transform const& obj, Matrix33::Matrix_33& x_form, Matrix33::Matrix_33 world_to_ndc_mat) {
	//Transform matrix here
	Matrix33::Matrix_33 result, scale_mat, rot_mat, trans_mat;

	Matrix_33Rot(rot_mat, 0);
	Matrix_33Scale(scale_mat, obj.scale.x, obj.scale.y);
	Matrix_33Translate(trans_mat, obj.position.x, obj.position.y);
	result = world_to_ndc_mat * trans_mat * rot_mat * scale_mat;

	// OpenGL requires matrix in col maj so transpose
	Matrix_33Transpose(x_form, result);
}

void Render::Manager::renderObject(Render::Shape const& e_shape) {
	//Set polygon mode
	glPolygonMode(GL_FRONT, GL_FILL);

	// use shader
	shader_system->useShader("base");

	//Shader set uniform
	shader_system->setUniform("base", "f_color", e_shape.color.color);
	shader_system->setUniform("base", "f_opacity", e_shape.color.alpha);
	shader_system->setUniform("base", "model_to_ndc", e_shape.x_form);

	//Get model
	auto model = NIKEEngine.accessAssets()->getModel(e_shape.model_ref);

	//Draw
	glBindVertexArray(model->vaoid);
	glDrawElements(model->primitive_type, model->draw_count, GL_UNSIGNED_INT, nullptr);

	//Unuse texture
	glBindVertexArray(0);
	shader_system->unuseShader();
}

void Render::Manager::renderObject(Render::Texture const& e_texture) {
	//Set polygon mode
	glPolygonMode(GL_FRONT, GL_FILL);

	// use shader
	shader_system->useShader("texture");

	//Texture unit
	constexpr int texture_unit = 6;

	// set texture
	glBindTextureUnit(
		texture_unit, // texture unit (binding index)
		NIKEEngine.accessAssets()->getTexture(e_texture.texture_ref)
	);

	glTextureParameteri(NIKEEngine.accessAssets()->getTexture(e_texture.texture_ref), GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(NIKEEngine.accessAssets()->getTexture(e_texture.texture_ref), GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Caculate UV Offset
	Vector2 uv_offset{ e_texture.frame_index.x * e_texture.frame_size.x, e_texture.frame_index.y * e_texture.frame_size.y };

	//Translate UV offset to bottom left
	uv_offset.y = std::abs(1 - uv_offset.y - e_texture.frame_size.y);

	//Set uniforms for texture rendering
	shader_system->setUniform("texture", "u_tex2d", texture_unit);
	shader_system->setUniform("texture", "u_opacity", e_texture.color.alpha);
	shader_system->setUniform("texture", "u_transform", e_texture.x_form);
	shader_system->setUniform("texture", "uvOffset", uv_offset);
	shader_system->setUniform("texture", "frameSize", e_texture.frame_size);

	//Get model
	auto model = NIKEEngine.accessAssets()->getModel("square-texture");

	//Draw
	glBindVertexArray(model->vaoid);
	glDrawElements(model->primitive_type, model->draw_count, GL_UNSIGNED_INT, nullptr);

	//Unuse texture
	glBindVertexArray(0);
	shader_system->unuseShader();
}

void Render::Manager::renderTextureRaw(unsigned int tex_hdl) {
	//Set polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// use shader
	shader_system->useShader("texture");

	//Texture unit
	constexpr int texture_unit = 6;

	// set texture
	glBindTextureUnit(
		texture_unit, // texture unit (binding index)
		tex_hdl
	);

	glTextureParameteri(tex_hdl, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(tex_hdl, GL_TEXTURE_WRAP_T, GL_REPEAT);

	Matrix33::Matrix_33 xform = Matrix33::Matrix_33::Identity();
	xform *= 0.1f;
	xform.matrix_33[2][0] = 0.1f;
	xform.matrix_33[2][1] = 0.1f;
	
	const Vector2 uv_offset{ 0, 0 };
	const Vector2 frame_size{ 1, 1 };

	//Set uniforms for texture rendering
	shader_system->setUniform("texture", "u_tex2d", texture_unit);
	shader_system->setUniform("texture", "u_opacity", 1);
	shader_system->setUniform("texture", "u_transform", xform);
	shader_system->setUniform("texture", "uvOffset", uv_offset);
	shader_system->setUniform("texture", "frameSize", frame_size);

	//Get model
	auto model = NIKEEngine.accessAssets()->getModel("square-texture");

	//Draw
	glBindVertexArray(model->vaoid);
	glDrawElements(model->primitive_type, model->draw_count, GL_UNSIGNED_INT, nullptr);

	//Unuse texture
	glBindVertexArray(0);
	shader_system->unuseShader();
}

void Render::Manager::renderText(Render::Text const& e_text) {
	const unsigned int test_ch_tex_hdl = NIKEEngine.accessAssets()->getFont("basic").at('6').texture;
	renderTextureRaw(test_ch_tex_hdl);
	return;


	// Iterate through all characters in the string
	for (char c : e_text.text) {
		const Character& ch = NIKEEngine.accessAssets()->getFont(e_text.font_ref).at(c);
		const unsigned int ch_tex_hdl = ch.texture;

		//renderTextureRaw(NIKEEngine.accessAssets()->getTexture("duck"));
		renderTextureRaw(ch_tex_hdl);
	}
}

void Render::Manager::renderWireFrame(Matrix33::Matrix_33 const& x_form, Render::Color const& e_color) {
	//Set Polygon Mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Use shader
	shader_system->useShader("base");

	//Shader set uniform
	shader_system->setUniform("base", "f_color", e_color.color);
	shader_system->setUniform("base", "f_opacity", e_color.alpha);
	shader_system->setUniform("base", "model_to_ndc", x_form);

	//Get model
	auto model = NIKEEngine.accessAssets()->getModel("square");

	//Draw model
	glBindVertexArray(model->vaoid);
	glDrawElements(GL_LINE_LOOP, model->draw_count, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	//Unuse shader
	shader_system->unuseShader();

	//Reset Polygon Mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Render::Manager::transformAndRenderEntity(Entity::Type entity, bool debugMode) {
	auto& e_transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);

	//Check If Shape
	if (NIKEEngine.checkEntityComponent<Render::Shape>(entity)) {
		auto& e_shape = NIKEEngine.getEntityComponent<Render::Shape>(entity);

		// Transform matrix here
		transformMatrix(e_transform, e_shape.x_form, camera_system->getWorldToNDCXform());

		//Render Shape
		renderObject(e_shape);
	}
	else if (NIKEEngine.checkEntityComponent<Render::Texture>(entity)) {
		auto& e_texture = NIKEEngine.getEntityComponent<Render::Texture>(entity);

		// Transform matrix here
		transformMatrix(e_transform, e_texture.x_form, camera_system->getWorldToNDCXform());

		// Render Texture
		renderObject(e_texture);
	}

	if (debugMode) {
		Matrix33::Matrix_33 mtx_wireframe;
		//Calculate wireframe matrix
		transformMatrixDebug(e_transform, mtx_wireframe, camera_system->getWorldToNDCXform());

		// Render debugging wireframe
		Render::Color wire_frame_color{ { 1.0f, 0.0f, 0.0f }, 1.0f };
		renderWireFrame(mtx_wireframe, wire_frame_color);
	}
}

void Render::Manager::trackCamEntity(std::string const& cam_identifier) {
	camera_system->trackCamEntity(cam_identifier);
}

std::unique_ptr<Camera::System>& Render::Manager::getCamEntity() {
	return camera_system;
}

void Render::Manager::init() {
	//Create font system
	font_system = std::make_unique<Font::Manager>();

	//Create shader system
	shader_system = std::make_unique<Shader::Manager>();

	//Create camera system
	camera_system = std::make_unique<Camera::System>();

	//Init Camera ( Camera height defaulted at window height )
	camera_system->init(NIKEEngine.accessWindow()->getWindowSize().y);

	//GL enable opacity blending option
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

	//Update camera with set of cam entitiess
	camera_system->updateCameraEntities(std::move(cam_entities));

	//Update camera system
	camera_system->update();

	//Update all and render except camera entities
	for (auto& entity : entities) {
		//Check for text objects
		if (NIKEEngine.checkEntityComponent<Render::Text>(entity)) {
			renderText(NIKEEngine.getEntityComponent<Render::Text>(entity));
			continue;
		}

		//Skip camera
		if (NIKEEngine.checkEntityComponent<Render::Cam>(entity)) continue;

		//Transform and render object
		transformAndRenderEntity(entity, debug_mode);
	}

	// Render camera above all components
	for (auto& entity : entities) {
		if (NIKEEngine.checkEntityComponent<Render::Cam>(entity)) {
			transformAndRenderEntity(entity, debug_mode);
		}
	}
}
