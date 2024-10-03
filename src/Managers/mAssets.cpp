/*****************************************************************//**
 * \file   mAssets.cpp
 * \brief  Assets manager function definitions
 *
 * \author Bryan Lim
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mAssets.h"
#include "../headers/Core/Engine.h"
#include "../headers/Systems/sysAudio.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Components/cRender.h"

Assets::Manager::~Manager() {

	//Clear fonts
	for (auto font_textures : fonts_list) {
		for (auto font : font_textures.second) {
			glDeleteTextures(1, &font.second.texture);
		}
	}

	//Clear models
	for (auto& model : models_list) {
		glDeleteVertexArrays(1, &model.second->vaoid);
		glDeleteBuffers(1, &model.second->vboid);
		glDeleteBuffers(1, &model.second->eboid);
	}

	//Clear textures
	for (std::pair<std::string, unsigned int> texture : textures_list) {
		glDeleteTextures(1, &texture.second);
	}

	//Clear shaders
	for (auto& shader : shaders_list) {
		glDeleteProgram(shader.second);
	}

	//Clear audios
	for (auto& audio : audio_list) {
		audio.second->release();
	}

	//Clear audio groups
	for (auto& audio_groups : audio_group_list) {
		audio_groups.second->release();
	}
}

/*****************************************************************//**
* Font
*********************************************************************/

void Assets::Manager::registerFont(std::string const& font_id, std::string const& file_path, Vector2 const& pixel_sizes) {
	if (fonts_list.find(font_id) != fonts_list.end())
	{
		throw std::runtime_error("FONT ALREADY EXISTS");
	}

	fonts_list.insert({ font_id, NIKEEngine.accessSystem<Render::Manager>()->registerFont(file_path, pixel_sizes) });
}

std::unordered_map<unsigned char, Render::Character> const& Assets::Manager::getFont(std::string const& font_id) const {
	if (fonts_list.find(font_id) == fonts_list.end())
	{
		throw std::runtime_error("FONT DOES NOT EXISTS");
	}

	return fonts_list.at(font_id);
}

 /*****************************************************************//**
 * Rendering
 *********************************************************************/

void Assets::Manager::registerShader(std::string const& shader_id, const std::string& vtx_path, const std::string& frag_path) {
	if (shaders_list.find(shader_id) != shaders_list.end())
	{
		throw std::runtime_error("MODELS ALREADY EXISTS");
	}

	shaders_list.insert({ shader_id, NIKEEngine.accessSystem<Render::Manager>()->registerShader(shader_id, vtx_path, frag_path) });
}

bool Assets::Manager::checkShader(std::string const& shader_id) {

	return shaders_list.find(shader_id) != shaders_list.end();

}

unsigned int Assets::Manager::getShader(std::string const& shader_id) {
	if (shaders_list.find(shader_id) == shaders_list.end())
	{
		throw std::runtime_error("SHADER DOES NOT EXISTS");
	}

	return shaders_list.at(shader_id);
}

void Assets::Manager::registerModel(std::string const& model_id, std::string const& file_path) {
	if (models_list.find(model_id) != models_list.end())
	{
		throw std::runtime_error("MODELS ALREADY EXISTS");
	}

	models_list.insert({ model_id, NIKEEngine.accessSystem<Render::Manager>()->registerModel(file_path) });
}

bool Assets::Manager::checkModel(std::string const& model_id) {

	return models_list.find(model_id) != models_list.end();

}

std::shared_ptr<Render::Model> Assets::Manager::getModel(std::string const& model_id) {
	if (models_list.find(model_id) == models_list.end())
	{
		throw std::runtime_error("MODEL DOES NOT EXISTS");
	}
	return models_list.at(model_id);
}

void Assets::Manager::registerTexture(std::string const& texture_id, std::string const& file_path) {
	if (textures_list.find(texture_id) != textures_list.end())
	{
		throw std::runtime_error("TEXTURES ALREADY EXISTS");
	}

	textures_list.insert({ texture_id, NIKEEngine.accessSystem<Render::Manager>()->registerTexture(file_path) });
}

unsigned int Assets::Manager::getTexture(std::string const& texture_id) {
	if (textures_list.find(texture_id) == textures_list.end())
	{
		throw std::runtime_error("TEXTURE DOES NOT EXISTS");
	}

	return textures_list.at(texture_id);
}

 /*****************************************************************//**
 * Audio
 *********************************************************************/

void Assets::Manager::registerSoundAudio(std::string const& file_path, std::string const& audio_tag)
{
	// Check if the audio already exists in the map
	if (audio_list.find(audio_tag) != audio_list.end())
	{
		throw std::runtime_error("AUDIO ALREADY EXISTS");
	}

	audio_list[audio_tag] = NIKEEngine.accessSystem<Audio::Manager>()->NEAudioLoadSound(file_path);
}

void Assets::Manager::registerMusicAudio(std::string const& file_path, std::string const& audio_tag)
{
	// Check if the audio already exists in the map
	if (audio_list.find(audio_tag) != audio_list.end())
	{
		throw std::runtime_error("AUDIO ALREADY EXISTS");
	}

	audio_list[audio_tag] = NIKEEngine.accessSystem<Audio::Manager>()->NEAudioLoadMusic(file_path);
}

std::shared_ptr<FMOD::Sound> Assets::Manager::getAudio(std::string const& audio_tag)
{
	// Check if the audio already exists in the map
	if (audio_list.find(audio_tag) == audio_list.end())
	{
		throw std::runtime_error("AUDIO DOES NOT EXISTS");
	}

	return audio_list.at(audio_tag);
}

void Assets::Manager::createAudioGroup(std::string const& audio_group_tag)
{	
	// Check if the group already exists in the map
	if (audio_group_list.find(audio_group_tag) != audio_group_list.end())
	{
		throw std::runtime_error("AUDIO GROUP ALREADY EXISTS"); 
	}

	// Push into audio group map
	audio_group_list[audio_group_tag] = NIKEEngine.accessSystem<Audio::Manager>()->CreateAudioGroup(audio_group_tag);
}

std::shared_ptr<FMOD::ChannelGroup> Assets::Manager::getAudioGroup(std::string const& tag)
{
	// Check if the group already exists in the map
	if (audio_group_list.find(tag) == audio_group_list.end())
	{
		throw std::runtime_error("AUDIO GROUP DOES NOT EXISTS");
	}

	return audio_group_list.at(tag);
}
