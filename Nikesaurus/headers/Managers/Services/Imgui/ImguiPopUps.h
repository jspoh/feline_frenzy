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
	bool showCreateEntityPopUp();

	// This function will create a pop up for user to input their entity name for easier UI
	bool showCreateEntityPrefabPopUp(std::string const& file_path);

	// This function will create a pop up for user to choose which component to add 
	bool showAddComponentPopup(Entity::Type& entity, bool pop_up);

	// This function will create a pop up for user to choose which component to remove
	// bool removeComponentPopup(Entity::Type& entity, bool pop_up);

	// This function will create a pop up to tell user that the component has already been added to that entity
	bool showComponentExistsPopup();

	// This function will create a pop up to ask the user if the user wants either the entity or component to removed
	bool removeEntityPopup(std::string& entity);

	// This function will create a pop up if there is an invalid string input to remove entity
	bool showInvalidEntityPopup();
	// Function to display an error popup if input is invalid
	bool ShowErrorPopup();
	// Function to display a save confirmation popup
	bool ShowSaveConfirmationPopup();
	// Function to clone entity
	bool cloneEntityPopup();
	// Function to change layer
	bool changeLayerPopup(Entity::Type entity);

	// Function to save entity into scn file
	bool savePrefabPopup(Entity::Type entity);

	// Function to confirm to delete file
	bool showDeleteFilePopup(const std::string& key, const std::string& asset_type);

	// Function to confirm to delete all files
	bool showDeleteAllFilesPopup(const std::string& asset_type);

	// Function to show pop up to create audio channel
	bool createAudioChannelPopup();
	// Function to show pop up to create grid
	bool createGridPopup();
}

#endif