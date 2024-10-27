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

namespace NIKESAURUS {
	/*!********************************************************************
	* Pop up UI
	**********************************************************************/

	// This function will create a pop up for user to input their entity name for easier UI
	bool showCreateEntityPopUp(bool pop_up);

	// This function will create a pop up for user to choose which component to add 
	bool showAddComponentPopup(Entity::Type& entity, bool pop_up);

	// This function will create a pop up to tell user that the component has already been added to that entity
	bool showComponentExistsPopup();

}

#endif