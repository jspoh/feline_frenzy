/*****************************************************************//**
 * \file   ImguiUtils.h
 * \brief  imgui system
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (60%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (40%)
 * \Brief This file contains declarations of utility functions to be used
 * \date   September 2024
 * All content ? 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef IMGUI_WINDOWS
#define IMGUI_WINDOWS

namespace NIKE {

	/*!********************************************************************
	* Window UI
	**********************************************************************/
	// Docking space for imgui
	void imguiDockingSpace();

	// This function will design the UI for the file system window
	// void imguiFileSystemWindow();

	// This function will design the UI for the performance view window
	void imguiDebuggingWindow();

	// This function will design the UI to show all the components available
	void imguiEntityWindow();

	// This function will design the UI to show all the components available
	void imguiEntityComponentManagementWindow();

	// This function will design the UI to show the loaded assets
	void imguiShowLoadedAssetsWindow();

	// This function will design the UI to show the loaded assets
	void imguiShowLoadedLevelsWindow();

	// This function will design the UI to show the loaded assets
	void imguiLayerManagementWindow();

	// This function renders the game into the viewport of the imgui window
	void imguiShowGameViewport(bool& dispatch, unsigned int tex_id);

	// This functions allows for the control of the camera
	void imguiCameraControl();

}


#endif 