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

#include "sAudio.h"

namespace NIKESAURUS {
	namespace Assets
	{
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		class NIKESAURUS_API Service {
		private:
			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			////Maps to fonts
			//std::unordered_map<std::string, std::unordered_map<unsigned char, Render::Character>> fonts_list;

			////Map to models for render
			//std::unordered_map<std::string, std::shared_ptr<Render::Model>> models_list;

			//// Map to textures for render
			//std::unordered_map<std::string, unsigned int> textures_list;

			////Map to shaders for render
			//std::unordered_map<std::string, unsigned int> shaders_list;

			//Fmod System
			std::shared_ptr<Audio::IAudioSystem> audio_system;

			//Map of audios
			std::unordered_map<std::string, std::shared_ptr<Audio::IAudio>> audio_list;
		public:

			//Default constructor
			Service() = default;

			// Dtor
			~Service();

			//Configure Assets Manager
			void configAssets(std::shared_ptr<Audio::IAudioSystem> audio_sys);

			///*****************************************************************//**
			//* Font
			//*********************************************************************/
			////Register font
			//void registerFont(std::string const& font_id, std::string const& file_path, Vector2 const& pixel_sizes = { 0.0f, 48.0f });

			////Get font
			//std::unordered_map<unsigned char, Render::Character> const& getFont(std::string const& font_id) const;

			///*****************************************************************//**
			//* Render ( Texture, Model, Shaders )
			//*********************************************************************/

			////Load shaders
			//void registerShader(std::string const& shader_id, const std::string& vtx_path, const std::string& frag_path);

			////Check shader exits
			//bool checkShader(std::string const& model_id);

			////Get shader
			//unsigned int getShader(std::string const& shader_id);

			////Load model
			//void registerModel(std::string const& model_id, std::string const& file_path);

			////Check model exits
			//bool checkModel(std::string const& model_id);

			////Get model
			//std::shared_ptr<Render::Model> getModel(std::string const& model_id);

			////Load texture
			//void registerTexture(std::string const& texture_id, std::string const& file_path);

			////Get texture
			//unsigned int getTexture(std::string const& texture_id);

			/*****************************************************************//**
			* Audio
			*********************************************************************/
			// Load sound audio
			void loadSound(std::string const& file_path, std::string const& audio_tag);

			//Load music audio
			void loadMusic(std::string const& file_path, std::string const& audio_tag);

			//Unload Audio
			void unloadAudio(std::string const& audio_tag);

			//Get audio
			std::shared_ptr<Audio::IAudio> getAudio(std::string const& audio_tag);
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif
