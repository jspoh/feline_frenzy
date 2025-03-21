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
		case Types::Video:
			return "Video";
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

		//Set loadable
		asset_types[Types::Texture].set(Modes::Loadable, true);
		asset_types[Types::Model].set(Modes::Loadable, true);
		asset_types[Types::Font].set(Modes::Loadable, true);
		asset_types[Types::Music].set(Modes::Loadable, true);
		asset_types[Types::Sound].set(Modes::Loadable, true);
		asset_types[Types::Script].set(Modes::Loadable, true);
		asset_types[Types::Video].set(Modes::Loadable, true);

		//Set executables
		asset_types[Types::Scene].set(Modes::Executable, true);
		asset_types[Types::Prefab].set(Modes::Executable, true);
		asset_types[Types::Grid].set(Modes::Executable, true);

		//Set editable
		asset_types[Types::Scene].set(Modes::Editable, true);
		asset_types[Types::Prefab].set(Modes::Editable, true);
		asset_types[Types::Grid].set(Modes::Editable, true);
		asset_types[Types::Script].set(Modes::Editable, true);

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

		//Video extension
		addValidExtensions(".mpg");

		//Other extension
		addValidExtensions(".prefab");
		addValidExtensions(".scn");
		addValidExtensions(".grid");
		addValidExtensions(".lua");
		addValidExtensions(".json");

		//Add invalid keys
		addInvalidKeys("batched_");

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

		//Register Prefab loader
		registerLoader(Assets::Types::Prefab, [this](std::filesystem::path const& primary_path) {
			// Temp entity for the prefab loading
			Entity::Type temp_entity = NIKE_ECS_MANAGER->createEntity();
			NIKE_SERIALIZE_SERVICE->loadEntityFromFile(temp_entity, primary_path.string());
			return nullptr;
			});

		//Register Scripts loader
		registerLoader(Assets::Types::Script, [this](std::filesystem::path const& primary_path) {
			return std::make_shared<sol::load_result>(NIKE_LUA_SERVICE->loadScript(primary_path));
			});
	}

	std::string Assets::Service::registerAsset(std::string const& path, bool b_virtual) {

		if (b_virtual) {
			if (!isPathValid(path)) {
				NIKEE_CORE_WARN("Invalid path detected. Asset will not be registered.");
				return "";
			}
			auto asset_id = getIDFromPath(path);
			auto asset_type = getAssetType(NIKE_PATH_SERVICE->resolvePath(path));
			asset_registry[asset_id] = MetaData(asset_type, NIKE_PATH_SERVICE->resolvePath(path));

			return asset_id;
		}
		else {
			if (!isPathValid(path, false)) {
				NIKEE_CORE_WARN("Invalid path detected. Asset will not be registered.");
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
			//Unregister
			register_it = asset_registry.erase(register_it);
			//Uncache
			asset_cache.erase(asset_id);
		}
	}

	void Assets::Service::registerLoader(Types asset_type, LoaderFunc loader) {
		if (asset_loader.find(asset_type) != asset_loader.end()) {
			throw std::runtime_error("Loader already registered.");
		}

		asset_loader.emplace(asset_type, loader);
	}

	void Assets::Service::cacheAsset(std::string const& asset_id) {

		//Check if asset is loadable
		if (!isAssetLoadable(asset_id)) {
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

		//Check if asset is loadable
		if (!isAssetLoadable(asset_id)) {
			return;
		}

		//Check asset cache
		auto cache_it = asset_cache.find(asset_id);
		if (cache_it != asset_cache.end()) {
			cache_it = asset_cache.erase(cache_it);
		}
	}

	void Assets::Service::recacheAsset(std::string const& asset_id) {

		//Check if asset is loadable
		if (!isAssetLoadable(asset_id)) {
			return;
		}

		//Uncache asset
		uncacheAsset(asset_id);

		//Cache asset
		cacheAsset(asset_id);
	}

	void Assets::Service::getExecutable(std::string const& asset_id) {

		//Check if asset is a executable asset type
		if (!(asset_types[getAssetType(asset_id)].test(Modes::Executable))) {
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

	bool Assets::Service::isAssetLoadable(std::string const& asset_id) const {
		return asset_types.at(getAssetType(asset_id)).test(Modes::Loadable);
	}

	bool Assets::Service::isAssetExecutable(std::string const& asset_id) const {
		return asset_types.at(getAssetType(asset_id)).test(Modes::Executable);
	}

	bool Assets::Service::isAssetEditable(std::string const& asset_id) const {
		return asset_types.at(getAssetType(asset_id)).test(Modes::Editable);
	}

	Assets::Types Assets::Service::getAssetType(std::string const& asset_id) const {
		if (asset_registry.find(asset_id) == asset_registry.end()) {
			return Assets::Types::None;
		}

		return asset_registry.at(asset_id).type;
	}

	std::string Assets::Service::getAssetTypeString(std::string const& asset_id) const {
		if (asset_registry.find(asset_id) == asset_registry.end()) {
			return typeToString(Assets::Types::None);
		}

		return typeToString(asset_registry.at(asset_id).type);
	}

	Assets::Types Assets::Service::getAssetType(std::filesystem::path const& path) const {
		auto ext = path.extension().string();
		// constexpr size_t music_threshold = 5 * 1024 * 1024; // 5 MB
		if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".tex") {
			return Assets::Types::Texture;
		}
		else if (ext == ".model") {
			return Assets::Types::Model;
		}
		else if (ext == ".ttf") {
			return Assets::Types::Font;
		}
		else if (ext == ".wav") {
			auto filepath = path.string();
			std::transform(filepath.begin(), filepath.end(), filepath.begin(), [](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });

			if (filepath.find("assets\\audios\\music") != std::string::npos) {
				return Assets::Types::Music;
			}
			else {
				return Assets::Types::Sound;
			}
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
		else if (ext == ".mpg") {
			return Assets::Types::Video;
		}
		else {
			return Assets::Types::None;
		}
	}

	std::filesystem::path Assets::Service::getAssetPath(std::string const& asset_id) const {
		if (asset_registry.find(asset_id) == asset_registry.end()) {
			NIKEE_CORE_WARN("Asset not registered!");
			return std::filesystem::path();
		}

		return asset_registry.at(asset_id).primary_path;
	}

	std::vector<const char*> Assets::Service::getAssetRefs(Assets::Types type) const {
		std::vector<const char*> asset_refs = {""};
		for (auto it = asset_registry.begin(); it != asset_registry.end(); ++it) {

			//Check if the asset contains any invalid keys
			bool is_invalid = false;
			for (const auto& invalid_key : invalid_keys) {

				//If invalid key is found
				if (it->second.primary_path.filename().string().find(invalid_key) != std::string::npos) {
					is_invalid = true;
					break;
				}
			}

			//Skip invalid keys or mismatched types
			if (is_invalid || it->second.type != type) {
				continue;
			}

			//Push to assets refs
			asset_refs.push_back(it->first.c_str());
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

	std::set<std::string> Assets::Service::getValidExtensions() const {
		return valid_extensions;
	}

	void Assets::Service::addInvalidKeys(std::string const& key) {
		invalid_keys.insert(key);
	}

	std::set<std::string> Assets::Service::getInvalidKeys() const {
		return invalid_keys;
	}

	bool Assets::Service::isPathValid(std::string const& path, bool b_virtual) const {
		if (b_virtual) {
			auto ext = NIKE_PATH_SERVICE->resolvePath(path).extension().string();
			if (valid_extensions.find(ext) != valid_extensions.end()) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			auto ext = std::filesystem::path(path).extension().string();
			if (valid_extensions.find(ext) != valid_extensions.end()) {
				return true;
			}
			else {
				return false;
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
					NIKEE_CORE_WARN("Asset will not be registered. Invalid extension found: " + file.path().extension().string() + " In path: " + file.path().string());
					continue;
				}

				//Configure asset type
				Types asset_type = getAssetType(file.path());


				//Register asset
				registerAsset(file.path().string(), false);

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
				NIKEE_CORE_WARN("Asset will not be registered. Invalid extension found: " + file.path().extension().string() + " In path: " + file.path().string());
				continue;
			}

			//Configure asset type
			Types asset_type = getAssetType(file.path());

			//Register asset
			registerAsset(file.path().string(), false);

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
			data[id] = {
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

	void Assets::Service::reserializeAllAssets() {

		//Reserialize all assets
		for (auto const& asset_data : asset_registry) {
			try {
				switch (asset_data.second.type) {
				case Assets::Types::Prefab: {

					//Create maximum number of layers
					while (NIKE_SCENES_SERVICE->getLayerCount() < Scenes::MAXLAYERS) {
						NIKE_SCENES_SERVICE->createLayer();
					}

					//Create tempe entity
					auto temp = NIKE_ECS_MANAGER->createEntity();

					//Deserialize
					NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(temp, asset_data.first);

					//Serialize
					const auto comps = NIKE_ECS_MANAGER->getAllEntityComponents(temp);
					NIKE_SERIALIZE_SERVICE->savePrefab(comps, asset_data.second.primary_path.string(), NIKE_METADATA_SERVICE->getEntityDataCopy(temp).has_value() ? NIKE_METADATA_SERVICE->getEntityDataCopy(temp).value() : NIKE::MetaData::EntityData());

					break;
				}
				case Assets::Types::Scene: {
					//Clear scene here
					NIKE_SCENES_SERVICE->resetScene();

					//Deserialize
					NIKE_SERIALIZE_SERVICE->loadSceneFromFile(asset_data.second.primary_path.string());

					//Serialize
					NIKE_SERIALIZE_SERVICE->saveSceneToFile(asset_data.second.primary_path.string());

					//Clear scene here
					NIKE_SCENES_SERVICE->resetScene();

					break;
				}
				case Assets::Types::Grid: {
					//Json Data
					nlohmann::json data;

					//Open file stream
					std::fstream in_file(asset_data.second.primary_path, std::ios::in);

					//Read data from file
					in_file >> data;

					//Deserialize
					NIKE_MAP_SERVICE->deserialize(data);

					//Open file stream
					std::fstream out_file(asset_data.second.primary_path, std::ios::out | std::ios::trunc);

					//Store data
					out_file << NIKE_MAP_SERVICE->serialize().dump(4);

					break;
				}
				}
			}
			catch ([[maybe_unused]]std::exception const& e) {
				NIKEE_CORE_WARN("Unable to reserialize asset. Deleting asset.");
				std::filesystem::remove(asset_data.second.primary_path);
			}
		}

		//Back to original state
		NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::RESTART, ""));
	}
}
