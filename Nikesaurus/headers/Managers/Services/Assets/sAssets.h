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

		//Asset types
		enum class Types {
			None = 0,
			Texture,
			Model,
			Font,
			Music,
			Sound,
			Scene,
			Prefab,
			Grid,
			Script
		};

		//Asset service
		class NIKE_API Service {
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
			using LoaderFunc = std::function<std::shared_ptr<void>(std::filesystem::path const&)>;

			//List of valid extension
			std::set<std::string> valid_extensions;

			//List of key words to exclude
			std::set<std::string> invalid_keys;

			//List of executable asset type
			std::set<Types> executable_types;

			//Asset registry of meta data
			std::unordered_map<std::string, MetaData> asset_registry;

			//Asset loader
			std::unordered_map<Types, LoaderFunc> asset_loader;

			//Assets cache for storing assets ( Optionally change to weakptr for a more event driven approach )
			std::unordered_map<std::string, std::shared_ptr<void>> asset_cache;

			//Font loader
			std::unique_ptr<Assets::FontLoader> font_loader;

			//Render loader
			std::unique_ptr<Assets::RenderLoader> render_loader;

			//Audio loader
			std::shared_ptr<Audio::IAudioSystem> audio_system;

			//Conversion from type to string
			std::string typeToString(Types type) const;
		public:

			//Default constructor and destructor
			Service() = default;
			~Service() = default;

			//Initialization
			void init(std::shared_ptr<Audio::IAudioSystem> audio_sys);

			//Register asset
			std::string registerAsset(std::string const& path, bool b_virtual = true);

			//Unregister asset
			void unregisterAsset(std::string const& asset_id);

			//Register loader
			void registerLoader(Types asset_type, LoaderFunc loader);

			//Cache asset
			void cacheAsset(std::string const& asset_id);

			//Uncache asset
			void uncacheAsset(std::string const& asset_id);

			//Recache asset
			void recacheAsset(std::string const& asset_id);

			//Get asset
			template <typename T>
			std::shared_ptr<T> getAsset(std::string const& asset_id) {

				//Check if asset is a executable asset type
				if (executable_types.find(getAssetType(asset_id)) != executable_types.end()) {
					NIKEE_CORE_WARN("Wrong usage! For fetching executable type assets use getExecutable().");
					return nullptr;
				}

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

					//Return nullptr
					return nullptr;
				}

				//Load assset through registered loaded
				auto loader_it = asset_loader.find(meta_it->second.type);
				if (loader_it == asset_loader.end()) {

					//Return nullptr
					throw std::runtime_error("Loader not registered for asset type");
				}

				//Get loaded asset
				auto asset = loader_it->second(meta_it->second.primary_path);

				//Insert loaded asset into asset cache
				asset_cache.emplace(asset_id, asset);

				//Return asset
				return std::static_pointer_cast<T>(asset);
			}

			//Get executable
			void getExecutable(std::string const& asset_id);

			//Add asset type as executable
			void addTypeAsExecutable(Types type);

			//check if asset is executable type
			bool isAssetExecutableType(std::string const& asset_id) const;

			//Get asset type from registered asset id
			Types getAssetType(std::string const& asset_id) const;

			//Get asset type string from registered asset id
			std::string getAssetTypeString(std::string const& asset_id) const;

			//Get asset type from path
			Types getAssetType(std::filesystem::path const& path) const;

			//Get asset path from registered asset id
			std::filesystem::path getAssetPath(std::string const& asset_id) const;

			//Get all asset ref of type
			std::vector<const char*> getAssetRefs(Types type) const;

			//Check if asset is loaded from asset id
			bool isAssetCached(std::string const& asset_id) const;

			//Check if asset is loaded from file path
			bool isAssetCached(std::filesystem::path const& path) const;
	
			//Add valid extension
			void addValidExtensions(std::string const& ext);

			//Get all valid extensions
			std::set<std::string> getValidExtensions() const;

			//Add invalid keys
			void addInvalidKeys(std::string const& key);

			//Get all invalid keys
			std::set<std::string> getInvalidKeys() const;

			//Check for valid path
			bool isPathValid(std::string const& path, bool b_virtual = true) const;

			//Check for registration
			bool isAssetRegistered(std::string const& asset_id) const;

			//Get ref from path
			std::string getIDFromPath(std::string const& path, bool b_virtual = true) const;

			//Clear expired cache
			void clearCache();

			//Register all assets from directory tree
			void scanAssetDirectory(std::string const& virtual_path, bool b_diretory_tree = false);

			//Cache all assets from directory tree
			void cacheAssetDirectory(std::string const& virtual_path, bool b_diretory_tree = false);

			//Remove cache for all assets from directory tree
			void uncacheAssetDirectory(std::string const& virtual_path, bool b_diretory_tree = false);

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
