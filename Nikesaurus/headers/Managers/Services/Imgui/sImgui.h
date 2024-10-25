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

namespace NIKESAURUS {
	// All Caps to differentiate from imgui includes
	namespace IMGUI {
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		class Service
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

			// Assets file path
			std::filesystem::path assets_path = "C:\\Users\\User\\feline_frenzy\\Feline Frenzy\\assets";


		private:
			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;


		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}


#endif