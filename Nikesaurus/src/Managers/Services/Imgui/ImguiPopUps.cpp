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
		static int entity_counter_for_print = 1;
		// Popup for entity naming
		if (ImGui::BeginPopupModal("Create Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Enter a name for the new entity:");
			ImGui::InputText("##EntityNameInput", entity_name, IM_ARRAYSIZE(entity_name));

			if (ImGui::Button("OK") || NIKE_ENGINE.getService<Input::Service>()->isKeyTriggered(NIKE_KEY_ENTER)) {
				// Create entity function call
				Entity::Type new_id = NIKE_ENGINE.getService<Coordinator::Service>()->createEntity();
				// If empty string, assign default string
				if (entity_name[0] == '\0')
				{
					snprintf(entity_name, sizeof(entity_name), "entity_%04d", entity_counter_for_print++);
				}
				// Save entity_name string
				NIKE_ENGINE.getService<IMGUI::Service>()->addEntityRef(entity_name, new_id);

				// Reset entity_name for the next use
				memset(entity_name, 0, sizeof(entity_name));

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

	bool showAddComponentPopup(Entity::Type& entity, bool pop_up)
	{
        // Track if the component exists popup should be shown
        static bool show_exist_popup = false;  

        if (ImGui::BeginPopupModal("Add Component", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Select a component to add:");
            // ImGui::TextColored(ImVec4(1, 0, 0, 1), "WARNING: Adding the same component twice will throw an exception!");

            // Get coordinator manager, save typing more stuff
            auto manager = NIKE_ENGINE.getService<Coordinator::Manager>();

            // Iterate over all registered components
            for (const auto& component : manager->getAllComponentTypes()) {
                // Component.first is the string name of the component
                // Component.second is the actual component type

                // Display each component as a button
                if (ImGui::Button(component.first.c_str())) {
                    // Check if the entity already has this component
                    if (!manager->checkEntityComponent(entity, component.second)) {
                        // Add the default-constructed component to the entity
                        NIKE_ENGINE.getService<NIKESAURUS::Coordinator::Service>()
                            ->addDefEntityComponent(entity, component.second);

                        // Close the popup after adding the component
                        ImGui::CloseCurrentPopup();
                    }
                    else {
                        // Set flag to open the "Component Exists" popup
                        show_exist_popup = true;
                    }
                }
            }

            ImGui::EndPopup();
        }

        // Handle the "Component Exists" popup separately
        if (show_exist_popup) {
            // Trigger the popup to open
            ImGui::OpenPopup("Component Exists");  
            // Reset flag after opening
            show_exist_popup = false; 
        }

        if (showComponentExistsPopup()) {
            // Reset in case the popup was closed
            show_exist_popup = false;  
        }

        return pop_up;
	}

    bool showComponentExistsPopup()
    {
        bool is_closed = false;
        if (ImGui::BeginPopupModal("Component Exists", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("This component has already been added to the entity.");
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
                // Mark the popup as closed
                is_closed = true;
            }
            ImGui::EndPopup();
        }
        return is_closed;
    }
}