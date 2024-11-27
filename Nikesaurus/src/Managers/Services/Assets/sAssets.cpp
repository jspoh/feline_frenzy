/*****************************************************************//**
 * \file   sAssets.cpp
 * \brief  Assets manager function definitions
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu 
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Assets/sAssets.h"
#include "Managers/Services/sAudio.h"

namespace NIKE {

	/*****************************************************************//**
	* Assets Service
	*********************************************************************/
	std::string Assets::Service::typeToString(Types type) const {
		switch (type) {
		case Types::Texture:
			return "Texture";
			break;
		case Types::Model:
			return "Model";
			break;
		case Types::Font:
			return "Font";
			break;
		case Types::Music:
			return "Music";
			break;
		case Types::Sound:
			return "Sound";
			break;
		case Types::Scene:
			return "Scene";
			break;
		case Types::Prefab:
			return "Prefab";
			break;
		case Types::Grid:
			return "Grid";
			break;
		case Types::Script:
			return "Script";
			break;
		default:
			return "Unknown";
			break;
		}
	}

	void Assets::Service::init(std::shared_ptr<Audio::IAudioSystem> audio_sys) {
		font_loader = std::make_unique<Assets::FontLoader>();
		render_loader = std::make_unique<Assets::RenderLoader>();
		audio_system = audio_sys;

		//Add executable types
		executable_types.insert(Types::Scene);
		executable_types.insert(Types::Prefab);
		executable_types.insert(Types::Grid);
		executable_types.insert(Types::Script);

		//Texture extensions
		addValidExtensions(".png");
		addValidExtensions(".jpg");
		addValidExtensions(".jpeg");
		addValidExtensions(".tex");

		//Font extension
		addValidExtensions(".ttf");

		//Model extension
		addValidExtensions(".model");

		//Audio extension
		addValidExtensions(".wav");

		//Other extension
		addValidExtensions(".prefab");
		addValidExtensions(".scn");
		addValidExtensions(".grid");
		addValidExtensions(".lua");
		addValidExtensions(".json");

		//Register texture loader
		registerLoader(Assets::Types::Texture, [this](std::filesystem::path const& primary_path) {
			return std::make_shared<Texture>(render_loader->compileTexture(primary_path.string()));
			});

		//Register model loader
		registerLoader(Assets::Types::Model, [this](std::filesystem::path const& primary_path) {
			return std::make_shared<Model>(render_loader->compileModel(primary_path.string()));
			});

		//Register font loader
		registerLoader(Assets::Types::Font, [this](std::filesystem::path const& primary_path) {
			return std::make_shared<Font>(std::static_pointer_cast<Assets::NIKEFontLib>(font_loader->getFontLib())->generateFont(primary_path.string()));
			});

		//Register music loader
		registerLoader(Assets::Types::Music, [this](std::filesystem::path const& primary_path) {
			return audio_system->createStream(primary_path.string());
			});

		//Register Sound loader
		registerLoader(Assets::Types::Sound, [this](std::filesystem::path const& primary_path) {
			return audio_system->createSound(primary_path.string());
			});

		//Register Scene loader
		registerLoader(Assets::Types::Scene, [this](std::filesystem::path const& primary_path) {
			NIKE_SERIALIZE_SERVICE->loadSceneFromFile(primary_path.string());
			return nullptr;
			});
	}

	std::string Assets::Service::registerAsset(std::string const& path, bool b_virtual) {
		if(b_virtual) {
			if (!isPathValid(path)) {
				NIKEE_CORE_ERROR("Invalid path detected. Asset will not be registered.");
				return "";
			}
			auto asset_id = getIDFromPath(path);
			auto asset_type = getAssetType(NIKE_PATH_SERVICE->resolvePath(path));
			asset_registry[asset_id] = MetaData(asset_type, NIKE_PATH_SERVICE->resolvePath(path));

			return asset_id;
		}
		else {
			if (!isPathValid(path, false)) {
				NIKEE_CORE_ERROR("Invalid path detected. Asset will not be registered.");
				return "";
			}
			auto asset_id = getIDFromPath(path, false);
			auto asset_type = getAssetType(std::filesystem::path(path));
			asset_registry[asset_id] = MetaData(asset_type, path);

			return asset_id;
		}
	}

	void Assets::Service::unregisterAsset(std::string const& asset_id) {
		//Check asset registry
		auto register_it = asset_registry.find(asset_id);
		if (register_it != asset_registry.end()) {
			register_it = asset_registry.erase(register_it);
		}
	}

	void Assets::Service::registerLoader(Types asset_type, LoaderFunc loader) {
		if (asset_loader.find(asset_type) != asset_loader.end()) {
			throw std::runtime_error("Loader already registered.");
		}

		asset_loader.emplace(asset_type, loader);
	}

	void Assets::Service::cacheAsset(std::string const& asset_id) {

		//Check if asset is an executable
		if (executable_types.find(getAssetType(asset_id)) != executable_types.end()) {
			return;
		}

		//Check asset cache
		auto cache_it = asset_cache.find(asset_id);
		if (cache_it != asset_cache.end()) {
			return;
		}

		//Get asset meta data
		auto meta_it = asset_registry.find(asset_id);
		if (meta_it == asset_registry.end()) {
			return;
		}

		//Load assset through registered loaded
		auto loader_it = asset_loader.find(meta_it->second.type);
		if (loader_it == asset_loader.end()) {
			return;
		}

		//Get loaded asset
		auto asset = loader_it->second(meta_it->second.primary_path);

		//Insert loaded asset into asset cache
		asset_cache.emplace(asset_id, asset);
	}

	void Assets::Service::uncacheAsset(std::string const& asset_id) {
		//Check if asset is an executable
		if (executable_types.find(getAssetType(asset_id)) != executable_types.end()) {
			return;
		}

		//Check asset cache
		auto cache_it = asset_cache.find(asset_id);
		if (cache_it != asset_cache.end()) {
			cache_it = asset_cache.erase(cache_it);
		}
	}

	void Assets::Service::recacheAsset(std::string const& asset_id) {

		//Check if asset is an executable
		if (executable_types.find(getAssetType(asset_id)) != executable_types.end()) {
			return;
		}
		
		//Uncache asset
		uncacheAsset(asset_id);

		//Cache asset
		cacheAsset(asset_id);
	}

	void Assets::Service::getExecutable(std::string const& asset_id) {

		//Check if asset is a executable asset type
		if (executable_types.find(getAssetType(asset_id)) == executable_types.end()) {
			NIKEE_CORE_WARN("Wrong usage! For fetching normal type assets use getAsset<T>().");
			return;
		}

		//Get asset meta data
		auto meta_it = asset_registry.find(asset_id);
		if (meta_it == asset_registry.end()) {
			return;
		}

		//Load assset through registered loaded
		auto loader_it = asset_loader.find(meta_it->second.type);
		if (loader_it == asset_loader.end()) {

			//Return nullptr
			throw std::runtime_error("Loader not registered for asset type");
		}

		//Run executable
		loader_it->second(meta_it->second.primary_path);
	}

	void Assets::Service::addTypeAsExecutable(Types type) {
		executable_types.insert(type);
	}

	bool Assets::Service::isAssetExecutableType(std::string const& asset_id) const {
		return executable_types.find(getAssetType(asset_id)) != executable_types.end();
	}

	Assets::Types Assets::Service::getAssetType(std::string const& asset_id) const {
		if (asset_registry.find(asset_id) == asset_registry.end()) {
			throw std::runtime_error("Asset not yet registered.");
		}

		return asset_registry.at(asset_id).type;
	}

	std::string Assets::Service::getAssetTypeString(std::string const& asset_id) const {
		if (asset_registry.find(asset_id) == asset_registry.end()) {
			throw std::runtime_error("Asset not yet registered.");
		}

		return typeToString(asset_registry.at(asset_id).type);
	}

	Assets::Types Assets::Service::getAssetType(std::filesystem::path const& path) const {
		auto ext = path.extension().string();
		constexpr size_t music_threshold = 5 * 1024 * 1024; // 5 MB
		if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".tex") {
			return Assets::Types::Texture;
		}
		else if (ext == ".model") {
			return Assets::Types::Model;
		}
		else if (ext == ".ttf") {
			return Assets::Types::Font;
		}
		else if (ext == ".wav" && std::filesystem::file_size(path) >= music_threshold) {
			return Assets::Types::Music;
		}
		else if (ext == ".wav" && std::filesystem::file_size(path) < music_threshold) {
			return Assets::Types::Sound;
		}
		else if (ext == ".scn") {
			return Assets::Types::Scene;
		}
		else if (ext == ".prefab") {
			return Assets::Types::Prefab;
		}
		else if (ext == ".grid") {
			return Assets::Types::Grid;
		}
		else if (ext == ".lua") {
			return Assets::Types::Script;
		}
		else {
			return Assets::Types::None;
		}
	}

	std::filesystem::path Assets::Service::getAssetPath(std::string const& asset_id) const {
		if (asset_registry.find(asset_id) == asset_registry.end()) {
			throw std::runtime_error("Asset not yet registered.");
		}

		return asset_registry.at(asset_id).primary_path;
	}

	std::vector<const char*> Assets::Service::getAssetRefs(Assets::Types type) const {
		std::vector<const char*> asset_refs;
		for (auto it = asset_registry.begin(); it != asset_registry.end(); ++it) {
			if (it->second.type == type) {
				asset_refs.push_back(it->first.c_str());
			}
		}

		return asset_refs;
	}

	bool Assets::Service::isAssetCached(std::string const& asset_id) const {

		//Check asset cache
		auto cache_it = asset_cache.find(asset_id);
		if (cache_it != asset_cache.end()) {
			return true;
		}

		return false;
	}

	bool Assets::Service::isAssetCached(std::filesystem::path const& path) const {
		auto id = getIDFromPath(path.string(), false);

		//Check asset cache
		auto cache_it = asset_cache.find(id);
		if (cache_it != asset_cache.end()) {
			return true;
		}

		return false;
	}

	void Assets::Service::addValidExtensions(std::string const& ext) {
		valid_extensions.insert(ext);
	}

	bool Assets::Service::isPathValid(std::string const& path, bool b_virtual) const {
		if (b_virtual) {
			auto ext = NIKE_PATH_SERVICE->resolvePath(path).extension().string();
			if (valid_extensions.find(ext) == valid_extensions.end()) {
				NIKEE_CORE_ERROR("Invalid extension found: " + ext + " In path: " + path);
				return false;
			}
			else {
				return true;
			}
		}
		else {
			auto ext = std::filesystem::path(path).extension().string();
			if (valid_extensions.find(ext) == valid_extensions.end()) {
				NIKEE_CORE_ERROR("Invalid extension found: " + ext + " In path: " + path);
				return false;
			}
			else {
				return true;
			}
		}
	}

	bool Assets::Service::isAssetRegistered(std::string const& asset_id) const {
		return asset_registry.find(asset_id) != asset_registry.end();
	}

	std::string Assets::Service::getIDFromPath(std::string const& path, bool b_virtual) const {

		if (b_virtual) {
			auto actual_path = NIKE_PATH_SERVICE->normalizePath(NIKE_PATH_SERVICE->resolvePath(path)).string();
			//string variables
			size_t start = actual_path.find_last_of('\\') + 1;
			//size_t size = actual_path.find_first_of('.', start) - start;
			std::string asset_id = actual_path.substr(start);

			return asset_id;
		}
		else {
			auto actual_path = NIKE_PATH_SERVICE->normalizePath(path).string();
			//string variables
			size_t start = actual_path.find_last_of('\\') + 1;
			//size_t size = actual_path.find_first_of('.', start) - start;
			std::string asset_id = actual_path.substr(start);

			return asset_id;
		}
	}

	void Assets::Service::clearCache() {
		//Clear asset cache when needed
		asset_cache.clear();
	}

	void Assets::Service::scanAssetDirectory(std::string const& virtual_path, bool b_diretory_tree) {

		//Resolve path
		auto root_path = NIKE_PATH_SERVICE->resolvePath(virtual_path);

		//Scan just root path
		if (!b_diretory_tree) {
			for (const auto& file : std::filesystem::directory_iterator(root_path)) {
				if (!file.is_regular_file()) continue;

				//Check for valid path before registering
				if (valid_extensions.find(file.path().extension().string()) == valid_extensions.end()) {
					NIKEE_CORE_ERROR("Asset will not be registered. Invalid extension found: " + file.path().extension().string() + " In path: " + file.path().string());
					continue;
				}

				//Configure asset type
				Types asset_type = getAssetType(file.path());

				//Add into registry
				asset_registry[getIDFromPath(file.path().string(), false)] = MetaData(asset_type, NIKE_PATH_SERVICE->normalizePath(file.path()));

				//Log registration
				NIKEE_CORE_INFO("Succesfully registered " + getIDFromPath(file.path().string(), false) + " Asset Type: " + typeToString(asset_type));
			}

			return;
		}

		//Scan root & directory tree
		for (const auto& file : std::filesystem::recursive_directory_iterator(root_path)) {
			if (!file.is_regular_file()) continue;

			//Check for valid path before registering
			if (valid_extensions.find(file.path().extension().string()) == valid_extensions.end()) {
				NIKEE_CORE_ERROR("Asset will not be registered. Invalid extension found: " + file.path().extension().string() + " In path: " + file.path().string());
				continue;
			}

			//Configure asset type
			Types asset_type = getAssetType(file.path());

			//Add into registry
			asset_registry[getIDFromPath(file.path().string(), false)] = MetaData(asset_type, NIKE_PATH_SERVICE->normalizePath(file.path()));

			//Log registration
			NIKEE_CORE_INFO("Succesfully registered " + getIDFromPath(file.path().string(), false) + " Asset Type: " + typeToString(asset_type));
		}
	}

	void Assets::Service::cacheAssetDirectory(std::string const& virtual_path, bool b_diretory_tree) {

		//Resolve path
		auto root_path = NIKE_PATH_SERVICE->resolvePath(virtual_path);

		//Load just root path
		if (!b_diretory_tree) {
			for (const auto& file : std::filesystem::directory_iterator(root_path)) {
				if (!file.is_regular_file()) continue;

				//Check for valid path before caching
				if (valid_extensions.find(file.path().extension().string()) == valid_extensions.end()) {
					continue;
				}

				//Get asset id
				auto asset_id = getIDFromPath(file.path().string(), false);

				//Cache asset
				cacheAsset(asset_id);
			}

			return;
		}

		//Load root & directory tree
		for (const auto& file : std::filesystem::recursive_directory_iterator(root_path)) {
			if (!file.is_regular_file()) continue;

			//Check for valid path before caching
			if (valid_extensions.find(file.path().extension().string()) == valid_extensions.end()) {
				continue;
			}

			//Get asset id
			auto asset_id = getIDFromPath(file.path().string(), false);

			//Cache asset
			cacheAsset(asset_id);
		}
	}

	void Assets::Service::uncacheAssetDirectory(std::string const& virtual_path, bool b_diretory_tree) {

		//Resolve path
		auto root_path = NIKE_PATH_SERVICE->resolvePath(virtual_path);

		//Load just root path
		if (!b_diretory_tree) {
			for (const auto& file : std::filesystem::directory_iterator(root_path)) {
				if (!file.is_regular_file()) continue;

				//Check for valid path before uncaching
				if (valid_extensions.find(file.path().extension().string()) == valid_extensions.end()) {
					continue;
				}

				//Get asset id
				auto asset_id = getIDFromPath(file.path().string(), false);

				//Uncache asset
				uncacheAsset(asset_id);
			}

			return;
		}

		//Load root & directory tree
		for (const auto& file : std::filesystem::recursive_directory_iterator(root_path)) {
			if (!file.is_regular_file()) continue;

			//Check for valid path before uncaching
			if (valid_extensions.find(file.path().extension().string()) == valid_extensions.end()) {
				continue;
			}

			//Get asset id
			auto asset_id = getIDFromPath(file.path().string(), false);

			//Uncache asset
			uncacheAsset(asset_id);
		}
	}

	void Assets::Service::logAssetsRegistry() const {
		for (auto const& asset : asset_registry) {
			cout << "ID: " << asset.first << " Path: " << asset.second.primary_path << endl;
		}
	}

	nlohmann::json Assets::Service::serialize() const {
		nlohmann::json data;

		//Serialize registry meta data
		for (const auto& [id, metadata] : asset_registry) {
			data[id] =	{
						{"Type", static_cast<int>(metadata.type)},
						{"Primary_Path", metadata.primary_path.string()},
						};
		}

		return data;
	}

	void Assets::Service::deserialize(nlohmann::json const& data) {

		//Deserialize registry meta data
		for (const auto& [id, meta_data] : data.items()) {
			asset_registry[id] = MetaData(static_cast<Types>(meta_data["Type"].get<int>()), meta_data["Primary_Path"].get<std::string>());
		}
	}
}
