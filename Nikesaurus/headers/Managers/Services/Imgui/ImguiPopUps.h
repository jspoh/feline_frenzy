/*****************************************************************//**
 * \file   ImguiPopUps.h
 * \brief  imgui system
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef IMGUI_POP_UPS
#define IMGUI_POP_UPS

namespace NIKE {
	/*!********************************************************************
	* Pop up UI
	**********************************************************************/

	// This function will create a pop up for user to input their entity name for easier UI
	bool showCreateEntityPopUp(bool pop_up);

	// This function will create a pop up for user to choose which component to add 
	bool showAddComponentPopup(Entity::Type& entity, bool pop_up);

	// This function will create a pop up to tell user that the component has already been added to that entity
	bool showComponentExistsPopup();
	
	// This function will create a pop up to ask the user if the user wants the asset to be loaded
	bool showLoadAssetPopup(const std::string& asset_name);

	// This function will create a pop up to ask the user if the user wants either the entity or component to removed
	bool removeEntityPopup();

	// This function will create a pop up if there is an invalid string input to remove entity
	bool showInvalidEntityPopup();

}

#endif