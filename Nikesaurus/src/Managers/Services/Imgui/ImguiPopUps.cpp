/*****************************************************************//**
 * \file   ImguiPopUps.cpp
 * \brief  imgui system
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/Imgui/sImgui.h"
#include "Core/Engine.h"

namespace NIKESAURUS
{
	bool showCreateEntityPopUp(bool pop_up)
	{
		static char entity_name[32];
		// Popup for entity naming
		if (ImGui::BeginPopupModal("Create Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Enter a name for the new entity:");
			ImGui::InputText("##EntityNameInput", entity_name, IM_ARRAYSIZE(entity_name));

			if (ImGui::Button("OK") || NIKEEngine.getService<Input::Service>()->isKeyTriggered(NIKE_KEY_ENTER)) {
				// Create entity function call
				Entity::Type new_id = NIKEEngine.getService<Coordinator::Service>()->createEntity();
				// Save entity_name string
				NIKEEngine.getService<IMGUI::Service>()->addEntityRef(entity_name, new_id);
				pop_up = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel")) {
				pop_up = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		return pop_up;
	}

	void showAddComponentPopup(Entity::Type& entity)
	{

	}
}