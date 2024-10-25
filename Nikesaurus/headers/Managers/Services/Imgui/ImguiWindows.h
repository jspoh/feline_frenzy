/*****************************************************************//**
 * \file   ImguiUtils.h
 * \brief  imgui system
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \Brief This file contains declarations of utility functions to be used
 * \date   September 2024
 * All content ? 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef IMGUI_WINDOWS
#define IMGUI_WINDOWS

namespace NIKESAURUS {

	/*!********************************************************************
	* Window UI
	**********************************************************************/
	// This function will design the UI for the file system window
	void imguiFileSystemWindow();
	// This function will design the UI to show all the components available
	void imguiEntityWindow();
	// This function will design the UI to show all the components available
	void imguiEntityComponentManagementWindow();
}


#endif 