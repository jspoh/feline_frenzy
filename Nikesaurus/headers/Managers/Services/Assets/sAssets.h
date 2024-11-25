/*****************************************************************//**
 * \file   sAssets.h
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

		//File Drop Event
		struct NIKE_API FileDropEvent : public Events::IEvent {
			int count;
			const char** paths;

			FileDropEvent(int count, const char** paths)
				: count{ count }, paths{ paths } {}
		};

		class NIKE_API Service 
			: public Events::IEventListener<FileDropEvent> 
		{
		private:
			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			// On drop event
			void onEvent(std::shared_ptr<FileDropEvent> event) override;

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

			//Map of audios split into sfx and music
			std::unordered_map<std::string, std::shared_ptr<Audio::IAudio>> sfx_list;
			std::unordered_map<std::string, std::shared_ptr<Audio::IAudio>> music_list;

			/*****************************************************************//**
			* File Paths for specific asset types
			*********************************************************************/
			std::string texture_path = "assets\\Textures\\";
			std::string sfx_path = "assets\\Audios\\SFX";
			std::string music_path = "assets\\Audios\\Music";
			std::string font_path = "assets\\Fonts\\";
			std::string models_path = "assets\\Models\\";
			std::string scenes_path = "assets\\Scenes\\";
			std::string shaders_path = "assets\\Shaders\\";
			std::string prefabs_path = "assets\\Prefabs\\";
			std::string scripts_path = "assets\\Scripts\\";
			std::string grid_path = "assets\\Grids\\";

			/*****************************************************************//**
			* Scn (Levels) private members
			*********************************************************************/
			std::unordered_map<std::string, std::filesystem::path> levels_list;

			/*****************************************************************//**
			* Prefabs private members
			*********************************************************************/
			std::unordered_map<std::string, std::filesystem::path> prefabs_list;

			/*****************************************************************//**
			* Scripts private members
			*********************************************************************/
			//std::unordered_map<std::string, std::filesystem::path> scripts_list;

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

			//Reload font
			void reloadFont(std::string const& font_id, std::string const& file_path, Vector2f const& pixel_sizes = { 0.0f, 48.0f });

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
			//Load model
			void loadModel(std::string const& model_id, std::string const& file_path, bool for_batched_rendering = false);

			//Reload model
			void reloadModel(std::string const& model_id, std::string const& file_path, bool for_batched_rendering = false);

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

			//Reload texture
			void reloadTexture(std::string const& texture_id, std::string const& file_path);

			// Unload texture
			void unloadTexture(std::string const& texture_id);

			// Unload all textures
			void unloadAllTextures();

			//Get texture
			std::shared_ptr<Assets::Texture> getTexture(std::string const& texture_id);

			//Check if texture exist
			bool checkTextureExist(std::string const& texture_id);

			//Check if texture loaded
			const std::unordered_map<std::string, std::shared_ptr<Assets::Texture>>& getLoadedTextures();

			//Handle drop for textures
			void handleTextureDrop(const std::filesystem::path& src_file_path);

			/*****************************************************************//**
			* Audio
			*********************************************************************/
			// Load audio
			void loadSfx(std::string const& audio_id, std::string const& file_path);
			void loadMusic(std::string const& audio_id, std::string const& file_path);

			// Reload audio
			void reloadSfx(std::string const& audio_id, std::string const& file_path);
			void reloadMusic(std::string const& audio_id, std::string const& file_path);

			//Unload specific audio
			void unloadSfx(const std::string& audio_id);
			void unloadMusic(const std::string& audio_id);

			//Unload all audios
			void unloadAllAudios();

			//Get audio
			std::shared_ptr<Audio::IAudio> getSfx(std::string const& audio_tag);
			std::shared_ptr<Audio::IAudio> getMusic(std::string const& audio_tag);

			//Get audios
			const std::unordered_map<std::string, std::shared_ptr<Audio::IAudio>>& getLoadedSfx();
			const std::unordered_map<std::string, std::shared_ptr<Audio::IAudio>>& getLoadedMusic();

			//Check if audio exits
			bool checkSfxExist(std::string const& audio_tag);
			bool checkMusicExist(std::string const& audio_tag);

			//Handle drop for textures
			void handleAudioDrop(const std::filesystem::path& src_file_path);

			/*****************************************************************//**
			* Scn File path
			*********************************************************************/
			void loadScn(const std::filesystem::directory_entry& entry);
			void loadScnFiles();
			bool checkScnFileExist(const std::string& entry);
			void reloadScn(std::string const& scn_key, std::filesystem::path const& scn_file_path);
			std::unordered_map<std::string, std::filesystem::path>& getLevelsList();

			/*****************************************************************//**
			* Prefab File path
			*********************************************************************/
			void loadPrefab(const std::filesystem::directory_entry& entry);
			void loadPrefabFiles();
			bool checkPrefabFileExist(const std::string& entry);
			void reloadPrefab(std::string const&, std::filesystem::path const&);
			std::unordered_map<std::string, std::filesystem::path>& getLoadedPrefabs();

			/*****************************************************************//**
			* Script File path
			*********************************************************************/
			//void loadScript(const std::filesystem::directory_entry& entry);
			//void loadScriptFiles();
			//bool checkScriptFileExist(const std::string& entry);
			//void reloadScript(std::string const&, std::filesystem::path const&);
			//std::unordered_map<std::string, std::filesystem::path>& getLoadedScripts();

			/*****************************************************************//**
			* File path gettors
			*********************************************************************/
			std::string const& getTexturePath(); 
			std::string const& getSfxPath(); 
			std::string const& getMusicPath();
			std::string const& getFontPath(); 
			std::string const& getModelsPath(); 
			std::string const& getScenesPath(); 
			std::string const& getShadersPath(); 
			std::string const& getPrefabsPath();
			std::string const& getScriptsPath();
			std::string const& getGridsPath();

			/*****************************************************************//**
			* Reload of specific asset types
			*********************************************************************/
			void reloadAssets(const std::string& asset_type);
			bool deleteFile(std::string const& file_path, const std::string& asset_type);
			bool deleteAllFiles(std::string const& file_path);
		};

		enum class Types {
			None = 0,
			Texture,
			Model,
			Font,
			Music,
			Sound,
		};

		class Services {
		private:

			//Assset meta data
			struct MetaData {
				Types type;
				std::filesystem::path primary_path;
				std::weak_ptr<void> cached;
				
				MetaData() : type{ 0 } {};
				MetaData(Types type, std::filesystem::path const& primary_path)
					: type{ type }, primary_path{ primary_path } {}
			};

			//Loader function
			using LoaderFunc = std::function<std::shared_ptr<void>(std::filesystem::path const&, std::filesystem::path const&)>;

			//Asset registry of meta data
			std::unordered_map<std::string, MetaData> asset_registry;

			//Asset loader
			std::unordered_map<Types, LoaderFunc> asset_loader;

			//Assets cache for storing assets ( To be changed to weak ptr next time once systems are event driven )
			std::unordered_map<std::string, std::shared_ptr<void>> asset_cache;

			//Font loader
			std::unique_ptr<Assets::FontLoader> font_loader;

			//Render loader
			std::unique_ptr<Assets::RenderLoader> render_loader;

			//Audio loader
			std::shared_ptr<Audio::IAudioSystem> audio_system;

			//Extension to asset type
			Types getAssetType(std::filesystem::path const& path) const;
		public:

			//Default constructor and destructor
			Services() = default;
			~Services() = default;

			//Initialization
			void init(std::shared_ptr<Audio::IAudioSystem> audio_sys);

			//Register asset
			void registerAsset(std::string const& asset_id, Types asset_type, std::string const& virtual_path);

			//Register loader
			void registerLoader(Types asset_type, LoaderFunc loader);

			//Get asset
			template <typename T>
			std::shared_ptr<T> getAsset(std::string const& asset_id) {
				//Check asset cache
				auto cache_it = asset_cache.find(asset_id);
				if (cache_it != asset_cache.end()) {
					if (cache_it->second) {
 						return std::static_pointer_cast<T>(cache_it->second);
					}
				}

				//Get asset meta data
				auto meta_it = asset_registry.find(asset_id);
				if (meta_it == asset_registry.end()) {
					throw std::runtime_error("Asset not registered yet.");
				}

				//Load assset through registered loaded
				auto loader_it = asset_loader.find(meta_it->second.type);
				if (loader_it == asset_loader.end()) {
					throw std::runtime_error("Loader not registered for type.");
				}

				//Get loaded asset
				auto asset = loader_it->second(meta_it->second.primary_path);

				//Insert loaded asset into asset cache
				asset_cache.emplace(asset_id, asset);

				//Return asset
				return std::static_pointer_cast<T>(asset);
			}

			//Clear expired cache
			void clearExpiredCache();

			//Register all assets from directory tree
			void scanAssetDirectory(std::filesystem::path const& root_path);

			//Log assets reigstry
			void logAssetsRegistry() const;

			//Serialize asset registry
			nlohmann::json serialize() const;

			//Deserialize asset registry
			void deserialize(nlohmann::json const& data);

		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif
