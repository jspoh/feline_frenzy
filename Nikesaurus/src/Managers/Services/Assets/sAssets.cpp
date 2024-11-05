/*****************************************************************//**
 * \file   mAssets.cpp
 * \brief  Assets manager function definitions
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Assets/sAssets.h"
#include "Managers/Services/sAudio.h"

namespace NIKE {
	Assets::Service::~Service() {

		//Clear fonts
		for (auto& font : fonts_list) {
			for (auto& chars : font.second->char_map) {
				glDeleteTextures(1, &chars.second.texture);
			}
		}

		//Clear shaders
		for (auto& shader : shaders_list) {
			glDeleteProgram(shader.second);
		}

		//Clear models
		for (auto& model : models_list) {
			glDeleteVertexArrays(1, &model.second->vaoid);
			glDeleteBuffers(1, &model.second->vboid);
			glDeleteBuffers(1, &model.second->eboid);
		}

		//Clear textures
		for (auto& texture : textures_list) {
			glDeleteTextures(1, &texture.second->gl_data);
		}

		//Clear audios
		audio_system = nullptr;
	}

	void Assets::Service::configAssets(std::shared_ptr<Audio::IAudioSystem> audio_sys) {
		font_loader = std::make_unique<Assets::FontLoader>();
		render_loader = std::make_unique<Assets::RenderLoader>();
		audio_system = audio_sys;
	}

	/*****************************************************************//**
	* Font
	*********************************************************************/
	void Assets::Service::loadFont(std::string const& font_id, std::string const& file_path, Vector2f const& pixel_sizes) {
		if (fonts_list.find(font_id) != fonts_list.end())
		{
			throw std::runtime_error("FONT ALREADY EXISTS");
		}

		NIKEE_CORE_INFO("Loading font to '" + font_id + "'");
		fonts_list.emplace(std::piecewise_construct, std::forward_as_tuple(font_id), std::forward_as_tuple(std::make_shared<Assets::Font>(std::static_pointer_cast<Assets::NIKEFontLib>(font_loader->getFontLib())->generateFont(file_path, pixel_sizes))));
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
		for (auto& chars : it->second->char_map) {
			glDeleteTextures(1, &chars.second.texture);
		}
		fonts_list.erase(it);
	}

	void Assets::Service::unloadAllFonts()
	{
		for (auto const& font : fonts_list)
		{
			unloadFont(font.first);
		}

		fonts_list.clear();
	}

	std::shared_ptr<Assets::Font> const& Assets::Service::getFont(std::string const& font_id) const {
		//Find shader
		auto it = fonts_list.find(font_id);

		//Check if shader exists
		if (it == fonts_list.end())
		{
			throw std::runtime_error("FONT DOES NOT EXISTS");
		}

		return it->second;
	}

	const std::unordered_map<std::string, std::shared_ptr<Assets::Font>>& Assets::Service::getLoadedFonts() const
	{
		return fonts_list;
	}

	bool Assets::Service::checkFontExist(std::string const& font_id)
	{
		if (fonts_list.find(font_id) == fonts_list.end())
		{
			return false;
		}
		return true;
	}

	/*****************************************************************//**
	* Rendering
	*********************************************************************/
	void Assets::Service::loadShader(std::string const& shader_id, const std::string& vtx_path, const std::string& frag_path) {
		if (shaders_list.find(shader_id) != shaders_list.end())
		{
			throw std::runtime_error("MODELS ALREADY EXISTS");
		}

		NIKEE_CORE_INFO("Loading shader to '" + shader_id + "'");
		shaders_list.emplace(std::piecewise_construct, std::forward_as_tuple(shader_id), std::forward_as_tuple(render_loader->compileShader(shader_id, vtx_path, frag_path)));
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

	void Assets::Service::unloadAllShaders()
	{
		for (const auto& shader : shaders_list)
		{
			unloadShader(shader.first);
		}

		shaders_list.clear();
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

	const std::unordered_map<std::string, unsigned int>& Assets::Service::getLoadedShaders()
	{
		return shaders_list;
	}

	void Assets::Service::loadModel(std::string const& model_id, std::string const& file_path) {
		if (models_list.find(model_id) != models_list.end())
		{
			throw std::runtime_error("MODELS ALREADY EXISTS");
		}

		NIKEE_CORE_INFO("Loading model to '" + model_id + "'");
		models_list.emplace(std::piecewise_construct, std::forward_as_tuple(model_id), std::forward_as_tuple(std::make_shared<Assets::Model>(render_loader->compileModel(file_path))));
	}

	void Assets::Service::unloadModel(std::string const& model_id) {
		//Find model
		auto it = models_list.find(model_id);

		// Check if the model exists in map
		if (it == models_list.end())
		{
			throw std::runtime_error("MODEL DOES NOT EXISTS");
		}

		//Unload model
		glDeleteVertexArrays(1, &it->second->vaoid);
		glDeleteBuffers(1, &it->second->vboid);
		glDeleteBuffers(1, &it->second->eboid);
		models_list.erase(it);
	}

	void Assets::Service::unloadAllModels()
	{
		for (auto const& model : models_list)
		{
			unloadModel(model.first);
		}

		models_list.clear();
	}

	std::shared_ptr<Assets::Model> Assets::Service::getModel(std::string const& model_id) {
		//Find model
		auto it = models_list.find(model_id);

		//Check if model exists
		if (it == models_list.end())
		{
			throw std::runtime_error("MODEL DOES NOT EXISTS");
		}

		return it->second;
	}

	const std::unordered_map<std::string, std::shared_ptr<Assets::Model>>& Assets::Service::getLoadedModels()
	{
		return models_list;
	}

	bool Assets::Service::checkModelExist(std::string const& model_id)
	{
		if (models_list.find(model_id) == models_list.end())
		{
			return false;
		}
		return true;
	}

	void Assets::Service::loadTexture(std::string const& texture_id, std::string const& file_path) {
		if (textures_list.find(texture_id) != textures_list.end())
		{
			throw std::runtime_error("TEXTURES ALREADY EXISTS");
		}

		NIKEE_CORE_INFO("Loading texture to '" + texture_id + "'");
		textures_list.emplace(std::piecewise_construct, std::forward_as_tuple(texture_id), std::forward_as_tuple(std::make_shared<Assets::Texture>(render_loader->compileTexture(file_path))));
	}

	void Assets::Service::unloadTexture(std::string const& texture_id) {
		//Find texture
		auto it = textures_list.find(texture_id);

		// Check if the texture exists in map
		if (it == textures_list.end())
		{
			throw std::runtime_error("TEXTURE DOES NOT EXISTS");
		}

		//Unload texture
		glDeleteTextures(1, &it->second->gl_data);
		textures_list.erase(it);
	}

	void Assets::Service::unloadAllTextures()
	{
		for (const auto& texture : textures_list)
		{
			unloadTexture(texture.first);
		}

		textures_list.clear();
	}

	std::shared_ptr<Assets::Texture> Assets::Service::getTexture(std::string const& texture_id) {
		if (textures_list.find(texture_id) == textures_list.end())
		{
			throw std::runtime_error("TEXTURE DOES NOT EXISTS");
		}

		return textures_list.at(texture_id);
	}

	bool Assets::Service::checkTextureLoaded(std::string const& texture_id)
	{
		if (textures_list.find(texture_id) == textures_list.end())
		{
			return false;
		}
		return true;
	}

	const std::unordered_map<std::string, std::shared_ptr<Assets::Texture>>& Assets::Service::getLoadedTextures()
	{
		return textures_list;
	}

	/*****************************************************************//**
	* Audio
	*********************************************************************/
	void Assets::Service::loadSound(std::string const& audio_id, std::string const& file_path)
	{
		// Check if the audio already exists in the map
		if (audio_list.find(audio_id) != audio_list.end())
		{
			throw std::runtime_error("AUDIO ALREADY EXISTS");
		}

		NIKEE_CORE_INFO("Loading sound to '" + audio_id + "'");
		//Emplace in audio list
		audio_list.emplace(std::piecewise_construct, std::forward_as_tuple(audio_id), std::forward_as_tuple(std::move(audio_system->createSound(file_path))));
	}

	void Assets::Service::loadMusic(std::string const& audio_id, std::string const& file_path)
	{
		// Check if the audio already exists in the map
		if (audio_list.find(audio_id) != audio_list.end())
		{
			throw std::runtime_error("AUDIO ALREADY EXISTS");
		}

		NIKEE_CORE_INFO("Loading music to '" + audio_id + "'");
		//Emplace in audio list
		audio_list.emplace(std::piecewise_construct, std::forward_as_tuple(audio_id), std::forward_as_tuple(std::move(audio_system->createStream(file_path))));
	}

	void Assets::Service::unloadAudio(std::string const& audio_id) {
		//Find audio
		auto it = audio_list.find(audio_id);

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

	void Assets::Service::unloadAllAudios()
	{
		for (const auto& audio : audio_list)
		{
			unloadAudio(audio.first);
		}

		audio_list.clear();
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

	const std::unordered_map<std::string, std::shared_ptr<Audio::IAudio>>& Assets::Service::getLoadedAudios()
	{
		return audio_list;
	}

	bool Assets::Service::checkAudioExist(std::string const& audio_tag)
	{
		if (audio_list.find(audio_tag) == audio_list.end())
		{
			return false;
		}
		return true;
	}

	std::string const& Assets::Service::getTexturePath() 
	{ 
		return texture_path; 
	}

	std::string const& Assets::Service::getAudioPath() 
	{
		return audio_path; 
	}

	std::string const& Assets::Service::getFontPath() 
	{
		return font_path;
	}

	std::string const& Assets::Service::getModelsPath() 
	{ 
		return models_path; 
	}

	std::string const& Assets::Service::getScenesPath() 
	{
		return scenes_path;
	}

	std::string const& Assets::Service::getShadersPath() 
	{
		return shaders_path; 
	}

	void Assets::Service::reloadAssets(const std::string& asset_type)
	{
		if (asset_type == "Textures") {
			// Load new textures
			for (const auto& texture_paths : std::filesystem::directory_iterator(getTexturePath())) {
				if (hasValidTextureExtension(texture_paths)) {
					std::string file_name = texture_paths.path().filename().string();

					// Check if the texture already exists before loading
					if (textures_list.find(file_name) == textures_list.end()) {
						loadTexture(file_name, texture_paths.path().string());
					}
				}
			}
		}
		else if (asset_type == "Audio") {

			// Load new audio
			for (const auto& audio_paths : std::filesystem::directory_iterator(getAudioPath())) {
				if (hasValidAudioExtension(audio_paths)) {
					std::string file_name = audio_paths.path().filename().string();

					// Check if the audio already exists before loading
					if (audio_list.find(file_name) == audio_list.end()) {
						loadMusic(file_name, audio_paths.path().string());
					}
				}
			}
		}
		else if (asset_type == "Fonts") {

			// Load new fonts
			for (const auto& font_paths : std::filesystem::directory_iterator(getFontPath())) {
				if (hasValidFontExtension(font_paths)) {
					std::string file_name = font_paths.path().filename().string();

					// Check if the font already exists before loading
					if (fonts_list.find(file_name) == fonts_list.end()) {
						loadFont(file_name, font_paths.path().string());
					}
				}
			}
		}
		else if (asset_type == "Models") {

			// Load new fonts
			for (const auto& model_paths : std::filesystem::directory_iterator(getModelsPath())) {
				if (hasValidModelExtension(model_paths)) {
					std::string file_name = model_paths.path().filename().string();

					// Check if the font already exists before loading
					if (models_list.find(file_name) == models_list.end()) {
						loadModel(file_name, model_paths.path().string());
					}
				}
			}
		}
		//else if (asset_type == "Shaders") {

		//	// Load new fonts
		//	for (const auto& shader_paths : std::filesystem::directory_iterator(getShadersPath())) {
		//		if (hasValidVertExtension(shader_paths)) {
		//			std::string file_name = shader_paths.path().filename().string();

		//			// Check if the font already exists before loading
		//			if (shaders_list.find(file_name) == shaders_list.end()) {
		//				loadShader(file_name, shader_paths.path().string());
		//			}
		//		}
		//	}
		//}

		// Others goes here
	}

}
