/*****************************************************************//**
 * \file   sysImgui.h
 * \brief  imgui system
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef IMGUI_HPP
#define IMGUI_HPP

#include "Managers/Services/Imgui/ImguiUtils.h"
#include "Managers/Services/Imgui/ImguiWindows.h"
#include "Managers/Services/Imgui/ImguiPopUps.h"

namespace NIKE {
	// All Caps to differentiate from imgui includes
	namespace IMGUI {
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		class NIKE_API Service
		{
		public:
			Service() = default;
			//Update
			void update();

			//Default Destructor
			~Service() = default;

			//Init
			void init();

			// Clean up
			void cleanUp();

			// Asset file path manipulation
			std::filesystem::path getAssetsPath();
			void setFilePath(const std::filesystem::path& path);

			// Settor to add an entity reference
			void addEntityRef(const std::string& name, Entity::Type entityType);

			// Gettors
			const std::unordered_map<std::string, Entity::Type>& getEntityRef() const;
			std::string& getSelectedEntityName();
			Entity::Type getEntityByName(std::string const& input);

		private:
			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			// Assets file path
			std::filesystem::path assets_path = "C:\\Users\\User\\feline_frenzy\\Feline Frenzy\\assets";
			// Container to store the entities created with string ref
			std::unordered_map<std::string, Entity::Type> entities_ref;
			// Variable to hold the selected entity name
			std::string selected_entity_name;

		};

		// Defines to reduce the long line
		#define GET_ASSETS_PATH() NIKE_ENGINE.NIKE_IMGUI_SERVICE->getAssetsPath()
		#define SET_ASSETS_PATH(path) NIKE_ENGINE.NIKE_IMGUI_SERVICE->setFilePath(path)

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}


#endif