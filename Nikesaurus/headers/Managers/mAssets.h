/*****************************************************************//**
 * \file   mAssets.h
 * \brief  Assets manager function declarations
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include "../headers/Systems/sysAudio.h"
#include "../headers/Components/cRender.h"

namespace Assets
{
	class Manager
	{
	public:

		//Default constructor
		Manager() = default;

		// Dtor
		~Manager();

		/*****************************************************************//**
		* Font
		*********************************************************************/
		//Register font
		void registerFont(std::string const& font_id, std::string const& file_path, Vector2 const& pixel_sizes = { 0.0f, 48.0f });

		//Get font
		std::unordered_map<unsigned char, Render::Character> const& getFont(std::string const& font_id) const;

		/*****************************************************************//**
		* Render ( Texture, Model, Shaders )
		*********************************************************************/

		//Load shaders
		void registerShader(std::string const& shader_id, const std::string& vtx_path, const std::string& frag_path);
		
		//Check shader exits
		bool checkShader(std::string const& model_id);

		//Get shader
		unsigned int getShader(std::string const& shader_id);

		//Load model
		void registerModel(std::string const& model_id, std::string const& file_path);

		//Check model exits
		bool checkModel(std::string const& model_id);

		//Get model
		std::shared_ptr<Render::Model> getModel(std::string const& model_id);

		//Load texture
		void registerTexture(std::string const& texture_id, std::string const& file_path);

		//Get texture
		unsigned int getTexture(std::string const& texture_id);

		/*****************************************************************//**
		* Audio
		*********************************************************************/

		// Load sound audio
		void registerSoundAudio(std::string const& file_path, std::string const& audio_tag);

		//Load music audio
		void registerMusicAudio(std::string const& file_path, std::string const& audio_tag);

		//Create audio group
		void createAudioGroup(std::string const& audio_group_tag);

		//Get audio
		std::shared_ptr<FMOD::Sound> getAudio(std::string const& audio_tag);

		//Get audio group
		std::shared_ptr<FMOD::ChannelGroup> getAudioGroup(std::string const& tag);

	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		//Maps to fonts
		std::unordered_map<std::string, std::unordered_map<unsigned char, Render::Character>> fonts_list;

		//Map to models for render
		std::unordered_map<std::string, std::shared_ptr<Render::Model>> models_list;

		// Map to textures for render
		std::unordered_map<std::string, unsigned int> textures_list;

		//Map to shaders for render
		std::unordered_map<std::string, unsigned int> shaders_list;

		//Map of audios
		std::unordered_map<std::string, Audio::NE_AUDIO> audio_list;

		//Map of audio groups
		std::unordered_map<std::string, Audio::NE_AUDIO_GROUP> audio_group_list;
	};
}

#endif