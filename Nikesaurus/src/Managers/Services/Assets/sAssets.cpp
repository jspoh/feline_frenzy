/*****************************************************************//**
 * \file   mAssets.cpp
 * \brief  Assets manager function definitions
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Assets/sAssets.h"
#include "Managers/Services/sAudio.h"

namespace NIKESAURUS {
	Assets::Service::~Service() {

		//Clear fonts
		for (auto& font : fonts_list) {
			for (auto& chars : font.second.char_map) {
				glDeleteTextures(1, &chars.second.texture);
			}
		}

		////Clear models
		//for (auto& model : models_list) {
		//	glDeleteVertexArrays(1, &model.second->vaoid);
		//	glDeleteBuffers(1, &model.second->vboid);
		//	glDeleteBuffers(1, &model.second->eboid);
		//}

		////Clear textures
		//for (std::pair<std::string, unsigned int> texture : textures_list) {
		//	glDeleteTextures(1, &texture.second);
		//}

		//Clear shaders
		for (auto& shader : shaders_list) {
			glDeleteProgram(shader.second);
		}

		//Clear audios
		audio_system = nullptr;
	}

	void Assets::Service::configAssets(std::shared_ptr<Audio::IAudioSystem> audio_sys) {
		font_system = std::make_unique<Font::Service>();
		audio_system = audio_sys;
	}

	///*****************************************************************//**
	//* Font
	//*********************************************************************/
	void Assets::Service::loadFont(std::string const& font_id, std::string const& file_path, Vector2f const& pixel_sizes) {
		if (fonts_list.find(font_id) != fonts_list.end())
		{
			throw std::runtime_error("FONT ALREADY EXISTS");
		}

		fonts_list.emplace(std::piecewise_construct, std::forward_as_tuple(font_id), std::forward_as_tuple(std::static_pointer_cast<Font::NIKFontLib>(font_system->getFontLib())->generateFont(file_path, pixel_sizes)));
	}

	void Assets::Service::unloadFont(std::string const& font_id) {
		//Find shader
		auto it = fonts_list.find(font_id);

		// Check if the shader exists in map
		if (it == fonts_list.end())
		{
			throw std::runtime_error("FONT DOES NOT EXISTS");
		}

		//Unload font
		for (auto& chars : it->second.char_map) {
			glDeleteTextures(1, &chars.second.texture);
		}
		fonts_list.erase(it);
	}

	Render::Font const& Assets::Service::getFont(std::string const& font_id) const {
		//Find shader
		auto it = fonts_list.find(font_id);

		//Check if shader exists
		if (it == fonts_list.end())
		{
			throw std::runtime_error("FONT DOES NOT EXISTS");
		}

		return it->second;
	}

	/*****************************************************************//**
	* Rendering
	*********************************************************************/

	unsigned int Assets::Service::compileShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path) {
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

	void Assets::Service::loadShader(std::string const& shader_id, const std::string& vtx_path, const std::string& frag_path) {
		if (shaders_list.find(shader_id) != shaders_list.end())
		{
			throw std::runtime_error("MODELS ALREADY EXISTS");
		}

		shaders_list.emplace(std::piecewise_construct, std::forward_as_tuple(shader_id), std::forward_as_tuple(compileShader(shader_id, vtx_path, frag_path)));
	}

	void Assets::Service::unloadShader(std::string const& shader_id) {
		//Find shader
		auto it = shaders_list.find(shader_id);

		// Check if the shader exists in map
		if (it == shaders_list.end())
		{
			throw std::runtime_error("SHADER DOES NOT EXISTS");
		}

		//Unload shader
		glDeleteProgram(it->second);
		shaders_list.erase(it);
	}

	unsigned int Assets::Service::getShader(std::string const& shader_id) {
		//Find shader
		auto it = shaders_list.find(shader_id);

		//Check if shader exists
		if (it == shaders_list.end())
		{
			throw std::runtime_error("SHADER DOES NOT EXISTS");
		}

		return it->second;
	}

	//void Assets::Manager::registerModel(std::string const& model_id, std::string const& file_path) {
	//	if (models_list.find(model_id) != models_list.end())
	//	{
	//		throw std::runtime_error("MODELS ALREADY EXISTS");
	//	}

	//	models_list.insert({ model_id, NIKEEngine.getService<Coordinator::Manager>()->accessSystem<Render::Manager>()->registerModel(file_path) });
	//}

	//bool Assets::Manager::checkModel(std::string const& model_id) {

	//	return models_list.find(model_id) != models_list.end();

	//}

	//std::shared_ptr<Render::Model> Assets::Manager::getModel(std::string const& model_id) {
	//	if (models_list.find(model_id) == models_list.end())
	//	{
	//		throw std::runtime_error("MODEL DOES NOT EXISTS");
	//	}
	//	return models_list.at(model_id);
	//}

	//void Assets::Manager::registerTexture(std::string const& texture_id, std::string const& file_path) {
	//	if (textures_list.find(texture_id) != textures_list.end())
	//	{
	//		throw std::runtime_error("TEXTURES ALREADY EXISTS");
	//	}

	//	textures_list.insert({ texture_id, NIKEEngine.getService<Coordinator::Manager>()->accessSystem<Render::Manager>()->registerTexture(file_path) });
	//}

	//unsigned int Assets::Manager::getTexture(std::string const& texture_id) {
	//	if (textures_list.find(texture_id) == textures_list.end())
	//	{
	//		throw std::runtime_error("TEXTURE DOES NOT EXISTS");
	//	}

	//	return textures_list.at(texture_id);
	//}

	/*****************************************************************//**
	* Audio
	*********************************************************************/
	void Assets::Service::loadSound(std::string const& file_path, std::string const& audio_tag)
	{
		// Check if the audio already exists in the map
		if (audio_list.find(audio_tag) != audio_list.end())
		{
			throw std::runtime_error("AUDIO ALREADY EXISTS");
		}

		//Emplace in audio list
		audio_list.emplace(std::piecewise_construct, std::forward_as_tuple(audio_tag), std::forward_as_tuple(std::move(audio_system->createSound(file_path))));
	}

	void Assets::Service::loadMusic(std::string const& file_path, std::string const& audio_tag)
	{
		// Check if the audio already exists in the map
		if (audio_list.find(audio_tag) != audio_list.end())
		{
			throw std::runtime_error("AUDIO ALREADY EXISTS");
		}

		//Emplace in audio list
		audio_list.emplace(std::piecewise_construct, std::forward_as_tuple(audio_tag), std::forward_as_tuple(std::move(audio_system->createStream(file_path))));
	}

	void Assets::Service::unloadAudio(std::string const& audio_tag) {
		//Find audio
		auto it = audio_list.find(audio_tag);

		// Check if the audio already exists in the map
		if (it == audio_list.end())
		{
			throw std::runtime_error("AUDIO DOES NOT EXISTS");
		}

		//Unload audio here
		//! STOP AUDIO HERE
		
		//Unload audio
		it->second->release();
		audio_list.erase(it);
	}

	std::shared_ptr<Audio::IAudio> Assets::Service::getAudio(std::string const& audio_tag)
	{
		//Find audio
		auto it = audio_list.find(audio_tag);

		// Check if the audio already exists in the map
		if (it == audio_list.end())
		{
			throw std::runtime_error("AUDIO DOES NOT EXISTS");
		}

		return it->second;
	}
}
