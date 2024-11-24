/*****************************************************************//**
 * \file   sPath.h
 * \brief  Path manager declarations
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef PATH_SERVICE_HPP
#define PATH_SERVICE_HPP

namespace NIKE {
	namespace Path {
		
		//Path global service
		class Service {
		private:

			//Storage of virtual paths
			std::unordered_map<std::string, std::filesystem::path> virtual_paths;

			//Event call back for file watchess
			using FileWatchEventCallback = std::function<void(const std::filesystem::path&, filewatch::Event)>;

			//Map of file watchers
			std::unordered_map<std::filesystem::path, std::unique_ptr<filewatch::FileWatch<std::string>>> dir_watchers;

			//Root directory path
			std::filesystem::path root_path;
		public:

			//Default constructor and destructor
			Service() = default;
			~Service() = default;

			//Init
			void init(nlohmann::json const& config);

			//Register new virtual path
			void registerVirtualPath(std::string const& path_alias, std::filesystem::path const& path, bool b_full_path = false);

			//Update virtual path
			void updateVirtualPath(std::string const& path_alias, std::filesystem::path const& path, bool b_full_path = false);

			//Resolve virtual path
			std::filesystem::path resolvePath(std::string const& virtual_path) const;

			//Normalise path
			std::filesystem::path normalizePath(std::filesystem::path const& path) const;

			//Log all registered virtual paths
			void logVirtualPaths() const;

			//Find root path
			std::filesystem::path findRootDirectory();

			//List directories
			std::vector<std::filesystem::path> listDirectories(std::string const& virtual_path) const;

			//List files
			std::vector<std::filesystem::path> listFiles(std::string const& virtual_path, std::string const& ext_filter = "") const;

			//Watch directory
			void watchDirectory(std::string const& virtual_path, FileWatchEventCallback callback);

			//Stop watching directory
			void stopWatchingDirectory(std::string const& virtual_path);

			//Stop watching all directories
			void stopWatchingAllDirectories();
		};
	}
}
#endif // !PATH_SERVICE_HPP
