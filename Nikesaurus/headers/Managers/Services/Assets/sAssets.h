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

			/*****************************************************************//**
			* File Paths for specific asset types
			*********************************************************************/

			std::string texture_path = "assets/Textures";
			std::string audio_path = "assets/Audios";
			std::string font_path = "assets/Fonts";
			std::string models_path = "assets/Models";
			std::string scenes_path = "assets/Scenes";
			std::string shaders_path = "assets/Shaders";

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

			//Unload all fonts
			void unloadAllFonts();

			//Get font
			std::shared_ptr<Assets::Font> const& getFont(std::string const& font_id) const;

			//Get loaded fonts
			const std::unordered_map<std::string, std::shared_ptr<Assets::Font>>& getLoadedFonts() const;

			// Checker
			bool checkFontExist(std::string const& font_id);

			/*****************************************************************//**
			* Render ( Texture, Model, Shaders )
			*********************************************************************/

			//Load shader
			void loadShader(std::string const& shader_id, const std::string& vtx_path, const std::string& frag_path);

			//Unload shader
			void unloadShader(std::string const& shader_id);

			//Unload all shaders
			void unloadAllShaders();

			//Get shader
			unsigned int getShader(std::string const& shader_id);

			//Get shaders
			const std::unordered_map<std::string, unsigned int>& getLoadedShaders();

			//Load model
			void loadModel(std::string const& model_id, std::string const& file_path);

			//Unload model
			void unloadModel(std::string const& model_id);

			//Unload all models
			void unloadAllModels();

			//Get model
			std::shared_ptr<Assets::Model> getModel(std::string const& model_id);

			//Get models
			const std::unordered_map<std::string, std::shared_ptr<Assets::Model>>& getLoadedModels();

			// Check model
			bool checkModelExist(std::string const& model_id);

			//Load texture
			void loadTexture(std::string const& texture_id, std::string const& file_path);

			// Unload texture
			void unloadTexture(std::string const& texture_id);

			// Unload all textures
			void unloadAllTextures();

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

			//Unload all audios
			void unloadAllAudios();

			//Get audio
			std::shared_ptr<Audio::IAudio> getAudio(std::string const& audio_tag);

			//Get audios
			const std::unordered_map<std::string, std::shared_ptr<Audio::IAudio>>& getLoadedAudios();

			bool checkAudioExist(std::string const& audio_tag);

			/*****************************************************************//**
			* File path gettors
			*********************************************************************/
			std::string const& getTexturePath(); 
			std::string const& getAudioPath(); 
			std::string const& getFontPath(); 
			std::string const& getModelsPath(); 
			std::string const& getScenesPath(); 
			std::string const& getShadersPath(); 

			/*****************************************************************//**
			* Reload of specific asset types
			*********************************************************************/
			void reloadAssets(const std::string& asset_type);
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif
