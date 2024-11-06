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

#include "sLoader.h"
#include "Managers/Services/sAudio.h"
#include "Components/cRender.h"

namespace NIKE {
	namespace Assets
	{
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		class NIKE_API Service {
		private:
			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			/*****************************************************************//**
			* Font Private Members
			*********************************************************************/

			//Free type lib
			std::unique_ptr<Assets::FontLoader> font_loader;

			//Maps to fonts
			std::unordered_map<std::string, std::shared_ptr<Assets::Font>> fonts_list;

			/*****************************************************************//**
			* Render Private Members
			*********************************************************************/
			std::unique_ptr<Assets::RenderLoader> render_loader;

			//Map to shaders for render
			std::unordered_map<std::string, unsigned int> shaders_list;

			//Map to models for render
			std::unordered_map<std::string, std::shared_ptr<Assets::Model>> models_list;

			// Map to textures for render
			std::unordered_map<std::string, std::shared_ptr<Assets::Texture>> textures_list;

			/*****************************************************************//**
			* Audio Private Members
			*********************************************************************/

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

			/*****************************************************************//**
			* Font
			*********************************************************************/
			//Register font
			void loadFont(std::string const& font_id, std::string const& file_path, Vector2f const& pixel_sizes = { 0.0f, 48.0f });

			//Unload font
			void unloadFont(std::string const& fond_id);

			//Get font
			std::shared_ptr<Assets::Font> const& getFont(std::string const& font_id) const;

			// Checker
			bool checkFontExist(std::string const& font_id);

			/*****************************************************************//**
			* Render ( Texture, Model, Shaders )
			*********************************************************************/

			//Load shader
			void loadShader(std::string const& shader_id, const std::string& vtx_path, const std::string& frag_path);

			//Unload shader
			void unloadShader(std::string const& shader_id);

			//Get shader
			unsigned int getShader(std::string const& shader_id);

			//Load model
			void loadModel(std::string const& model_id, std::string const& file_path, bool for_batched_rendering=false);

			//Unload model
			void unloadModel(std::string const& model_id);

			//Get model
			std::shared_ptr<Assets::Model> getModel(std::string const& model_id);

			// Check model
			bool checkModelExist(std::string const& model_id);

			//Load texture
			void loadTexture(std::string const& texture_id, std::string const& file_path);

			//Unload texture
			void unloadTexture(std::string const& texture_id);

			//Get texture
			std::shared_ptr<Assets::Texture> getTexture(std::string const& texture_id);

			//Check if texture loaded
			bool checkTextureLoaded(std::string const& texture_id);

			//Check if texture loaded
			const std::unordered_map<std::string, std::shared_ptr<Assets::Texture>>& getLoadedTextures();

			/*****************************************************************//**
			* Audio
			*********************************************************************/
			// Load sound audio
			void loadSound(std::string const& audio_id, std::string const& file_path);

			//Load music audio
			void loadMusic(std::string const& audio_id, std::string const& file_path);

			//Unload Audio
			void unloadAudio(std::string const& audio_id);

			//Get audio
			std::shared_ptr<Audio::IAudio> getAudio(std::string const& audio_tag);

			// Check for audio
			bool checkAudioExist(std::string const& audio_tag);
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif
