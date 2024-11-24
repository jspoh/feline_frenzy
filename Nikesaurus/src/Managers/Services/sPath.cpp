/*****************************************************************//**
 * \file   sPath.cpp
 * \brief  Path manager definitions
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/sPath.h"

namespace NIKE {

	void Path::Service::init(nlohmann::json const& config) {
		//Identify root directory
		root_path = findRootDirectory();

		try {
			auto const& data = config.at("PathsConfig");

			//Register default path aliases
			for (const auto& [alias, actual_path] : data.items()) {
				virtual_paths.emplace(alias, normalizePath(std::filesystem::path(root_path / actual_path.get<std::string>())));
			}
		}
		catch (const nlohmann::json::exception& e) {
			NIKEE_CORE_WARN(e.what());
			NIKEE_CORE_WARN("Paths config invalid! No default virtual path registered");
		}
	}

	void Path::Service::registerVirtualPath(std::string const& path_alias, std::filesystem::path const& path, bool b_full_path) {
		
		//Error already registered
		if (virtual_paths.find(path_alias) != virtual_paths.end()) {
			throw std::runtime_error("Path alias already registred");
		}

		//Normalised and correct path
		std::filesystem::path full_path = b_full_path ? normalizePath(path) : normalizePath(std::filesystem::path(root_path / path));

		//Emplace virtual path
		virtual_paths.emplace(path_alias, full_path);
	}

	void Path::Service::updateVirtualPath(std::string const& path_alias, std::filesystem::path const& path, bool b_full_path) {
		
		//Error not yet registered
		if (virtual_paths.find(path_alias) == virtual_paths.end()) {
			throw std::runtime_error("Path alias not registered");
		}

		//Normalised and correct path
		std::filesystem::path full_path = b_full_path ? normalizePath(path) : normalizePath(std::filesystem::path(root_path / path));

		//Emplace virtual path
		virtual_paths[path_alias] = normalizePath(full_path);
	}

	std::filesystem::path Path::Service::resolvePath(std::string const& virtual_path) const {
		
		//Check for valid virtual path
		auto separator_pos = virtual_path.find(":/");
		if (separator_pos == std::string::npos) {
			throw std::runtime_error("Invalid virtual path format: " + virtual_path);
		}

		//Get alias and relative path
		std::string alias = virtual_path.substr(0, separator_pos + 2);
		std::string relativePath = virtual_path.substr(separator_pos + 2);

		//Find alias
		auto it = virtual_paths.find(alias);
		if (it == virtual_paths.end()) {
			throw std::runtime_error("Unknown virtual path alias: " + alias);
		}

		//Return resolved path
		return it->second / relativePath;
	}

	std::filesystem::path Path::Service::normalizePath(std::filesystem::path const& path) const {
		return std::filesystem::weakly_canonical(path);
	}

	void Path::Service::logVirtualPaths() const {
		for (auto const& [alias, path] : virtual_paths) {
			NIKEE_CORE_INFO(alias + " -> " + path.string());
		}
	}

	std::filesystem::path Path::Service::findRootDirectory() {

		//Get current path
		auto current_path = std::filesystem::current_path();

		//Go through the file hierachy to find the root directory
		while (!current_path.empty()) {
			for (const auto& entry : std::filesystem::directory_iterator(current_path)) {

				//Check for directory with .sln file ( Root Directory )
				if (entry.is_regular_file() && entry.path().extension() == ".sln") {
					return current_path;
				}
			}

			//Move up the file hierachy
			current_path = current_path.parent_path();
		}

		//Root directory not found, file system error
		throw std::runtime_error("Error! Solution not present within any of the path hierachies.");
	}

	std::vector<std::filesystem::path> Path::Service::listDirectories(std::string const& virtual_path) const {
		//Get actual path
		auto actual_path = resolvePath(virtual_path);

		//Check if path exists
		if (!std::filesystem::exists(actual_path) || !std::filesystem::is_directory(actual_path)) {
			throw std::runtime_error("Path does not exist or is not a directory: " + actual_path.string());
		}

		//Store of directories in a virtual path
		std::vector<std::filesystem::path> directories;
		for (const auto& directory : std::filesystem::directory_iterator(actual_path)) {

			//Check for directory
			if (directory.is_directory()) {
				directories.push_back(directory.path());
			}
		}

		//Return all directories
		return directories;
	}

	std::vector<std::filesystem::path> Path::Service::listFiles(std::string const& virtual_path, std::string const& ext_filter) const {

		//Get actual path
		auto actual_path = resolvePath(virtual_path);

		//Check if path exists
		if (!std::filesystem::exists(actual_path) || !std::filesystem::is_directory(actual_path)) {
			throw std::runtime_error("Path does not exist or is not a directory: " + actual_path.string());
		}

		//Store of files in a virtual path
		std::vector<std::filesystem::path> files;
		for (const auto& file : std::filesystem::directory_iterator(actual_path)) {

			//Check for files with extension or if no extension filter present, extract all files
			if (file.is_regular_file() && (ext_filter.empty() || file.path().extension() == ext_filter)) {
				files.push_back(file.path());
			}
		}

		//Return all files
		return files;
	}

	void Path::Service::watchDirectory(std::string const& virtual_path, FileWatchEventCallback callback) {

		//Get actual path
		auto actual_path = resolvePath(virtual_path);

		//Check if path exists
		if (!std::filesystem::exists(actual_path) || !std::filesystem::is_directory(actual_path)) {
			throw std::runtime_error("Path does not exist or is not a directory: " + actual_path.string());
		}

		// Add watcher for the directory
		dir_watchers.emplace(actual_path,
			std::make_unique<filewatch::FileWatch<std::string>>(actual_path.string(),
			[callback](const std::string& file, filewatch::Event event) {
				callback(file, event);
			}));
	}

	void Path::Service::stopWatchingDirectory(std::string const& virtual_path) {
		//Get actual path
		auto actual_path = resolvePath(virtual_path);

		//Check if path exists
		if (!std::filesystem::exists(actual_path) || !std::filesystem::is_directory(actual_path)) {
			throw std::runtime_error("Path does not exist or is not a directory: " + actual_path.string());
		}

		//Erase directory from watchers
		auto it = dir_watchers.find(actual_path);
		if (it != dir_watchers.end()) {
			dir_watchers.erase(it);
		}
	}

	void Path::Service::stopWatchingAllDirectories() {
		dir_watchers.clear();
	}
}
