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
#include "Managers/Services/sAssets.h"
#include "Managers/Services/sAudio.h"

namespace NIKESAURUS {
	Assets::Service::~Service() {

		////Clear fonts
		//for (auto& font_textures : fonts_list) {
		//	for (auto& font : font_textures.second) {
		//		glDeleteTextures(1, &font.second.texture);
		//	}
		//}

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

		////Clear shaders
		//for (auto& shader : shaders_list) {
		//	glDeleteProgram(shader.second);
		//}

		//Clear audios
		for (auto& audio : audio_list) {
			audio.second->release();
		}

		//Clear audio groups
		for (auto& audio_groups : audio_group_list) {
			audio_groups.second->release();
		}
	}

	void Assets::Service::configAssets(FMOD::System* fmod_sys) {
		fmod_system = fmod_sys;
	}

	///*****************************************************************//**
	//* Font
	//*********************************************************************/

	//void Assets::Manager::registerFont(std::string const& font_id, std::string const& file_path, Vector2 const& pixel_sizes) {
	//	if (fonts_list.find(font_id) != fonts_list.end())
	//	{
	//		throw std::runtime_error("FONT ALREADY EXISTS");
	//	}

	//	fonts_list.insert({ font_id, NIKEEngine.getService<Coordinator::Manager>()->accessSystem<Render::Manager>()->registerFont(file_path, pixel_sizes)});
	//}

	//std::unordered_map<unsigned char, Render::Character> const& Assets::Manager::getFont(std::string const& font_id) const {
	//	if (fonts_list.find(font_id) == fonts_list.end())
	//	{
	//		throw std::runtime_error("FONT DOES NOT EXISTS");
	//	}

	//	return fonts_list.at(font_id);
	//}

	///*****************************************************************//**
	//* Rendering
	//*********************************************************************/

	//void Assets::Manager::registerShader(std::string const& shader_id, const std::string& vtx_path, const std::string& frag_path) {
	//	if (shaders_list.find(shader_id) != shaders_list.end())
	//	{
	//		throw std::runtime_error("MODELS ALREADY EXISTS");
	//	}

	//	shaders_list.insert({ shader_id,NIKEEngine.getService<Coordinator::Manager>()->accessSystem<Render::Manager>()->registerShader(shader_id, vtx_path, frag_path) });
	//}

	//bool Assets::Manager::checkShader(std::string const& shader_id) {

	//	return shaders_list.find(shader_id) != shaders_list.end();

	//}

	//unsigned int Assets::Manager::getShader(std::string const& shader_id) {
	//	if (shaders_list.find(shader_id) == shaders_list.end())
	//	{
	//		throw std::runtime_error("SHADER DOES NOT EXISTS");
	//	}

	//	return shaders_list.at(shader_id);
	//}

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
		FMOD::SoundGroup* sound_group;
		FMOD_RESULT result;
		result = it->second->getSoundGroup(&sound_group);

		//Stop sound group if part of a group
		if (result == FMOD_OK)
			sound_group->stop();
		
		//Unload audio
		it->second->release();
		audio_list.erase(it);
	}

	std::shared_ptr<FMOD::Sound> Assets::Service::getAudio(std::string const& audio_tag)
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

	void Assets::Service::createAudioGroup(std::string const& audio_group_tag)
	{
		// Check if the group already exists in the map
		if (audio_group_list.find(audio_group_tag) != audio_group_list.end())
		{
			throw std::runtime_error("AUDIO GROUP ALREADY EXISTS");
		}

		//FMOD Group loading Variables
		FMOD::ChannelGroup* temp = nullptr;
		FMOD_RESULT result;
		result = fmod_system->createChannelGroup(audio_group_tag.c_str(), &temp);
		if (result != FMOD_OK)
		{
			throw std::runtime_error("AUDIO GROUP NOT INITIALIZED");
		}

		//Emplace into audio group list
		audio_group_list.emplace(std::piecewise_construct, std::forward_as_tuple(audio_group_tag), std::forward_as_tuple(std::move(std::make_shared<FMOD::ChannelGroup>(temp, [](FMOD::ChannelGroup*) {}))));
	}

	std::shared_ptr<FMOD::ChannelGroup> Assets::Service::getAudioGroup(std::string const& audio_group_tag)
	{
		//Find Audio Group
		auto it = audio_group_list.find(audio_group_tag);

		// Check if the group already exists in the map
		if (it == audio_group_list.end())
		{
			throw std::runtime_error("AUDIO GROUP DOES NOT EXISTS");
		}

		return it->second;
	}
}
