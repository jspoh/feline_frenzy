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
#include "Managers/Services/sEvents.h"

namespace NIKE {
	// All Caps to differentiate from imgui includes
	namespace IMGUI {
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		//View port event
		struct ViewPortEvent : public Events::IEvent {
			Vector2f window_pos;
			Vector2f window_size;

			ViewPortEvent() : window_pos() {}
			ViewPortEvent(Vector2f const& window_pos, Vector2f const& window_size) : window_pos{ window_pos }, window_size{ window_size } {}
		};

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
			void addEntityRef(const std::string& name, Entity::Type entity_type);

			bool checkEntityExist(const std::string& entity_ref);

			// For reloading of specific asset type
			void reloadAssets(const std::string& asset_type);

			// Populate dropdown list whenever component is added
			bool populateLists = false;

			// Reset variables
			void resetVariables();

			// Setters
			void setGamePaused(bool pause);

			// Gettors
			std::unordered_map<std::string, Entity::Type>& getEntityRef();
			std::string& getSelectedEntityName();
			Entity::Type getEntityByName(std::string const& input);

			//Gettor for seeing if imgui is active
			bool getImguiActive() const;
			bool getGamePaused() const;

		private:
			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			// Assets file path
			std::filesystem::path assets_path = "assets/";
			// Container to store the entities created with string ref
			std::unordered_map<std::string, Entity::Type> entities_ref;
			// Variable to hold the selected entity name
			std::string selected_entity_name;

			//Boolean for toggling imgui
			bool b_show_imgui = false;
			bool b_dispatch_viewport = false;
			bool b_pause_game = false;
		};

		// Defines to reduce the long line
		#define GET_ASSETS_PATH() NIKE_ENGINE.NIKE_IMGUI_SERVICE->getAssetsPath()
		#define SET_ASSETS_PATH(path) NIKE_ENGINE.NIKE_IMGUI_SERVICE->setFilePath(path)

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}


#endif