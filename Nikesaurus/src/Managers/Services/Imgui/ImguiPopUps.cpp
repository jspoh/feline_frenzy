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
#include "Components/cRender.h"
#include "Components/cTransform.h"
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

			if (ImGui::Button("OK") || NIKEEngine.getService<Input::Service>()->isKeyTriggered(NIKE_KEY_ENTER)) {
				// Create entity function call
				Entity::Type new_id = NIKEEngine.getService<Coordinator::Service>()->createEntity();
				// If empty string, assign default string
				if (entity_name[0] == '\0')
				{
					snprintf(entity_name, sizeof(entity_name), "entity_%04d", entity_counter_for_print++);
				}
				// Save entity_name string
				NIKEEngine.getService<IMGUI::Service>()->addEntityRef(entity_name, new_id);

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
            auto manager = NIKEEngine.getService<Coordinator::Manager>();

            // Add Transform component
            if (ImGui::Button("Transform")) {
                if (!manager->checkEntityComponent<Transform::Transform>(entity)) {
                    // Add default ctored comp
                    NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addDefEntityComponent(entity, 
                       NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->getAllComponentTypes().at("Transform::Transform"));
                    // manager->addEntityComponent(entity, Transform::Transform());
                    ImGui::CloseCurrentPopup();
                }
                else {
                    // Flag to open the component exists popup
                    show_exist_popup = true;  
                }
            }

            ImGui::SameLine();

            // Add Texture component
            if (ImGui::Button("Texture")) {
                if (!manager->checkEntityComponent<Render::Texture>(entity)) {
                    // Add default ctored comp
                    NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->addDefEntityComponent(entity,
                        NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->getAllComponentTypes().at("Transform::Transform"));
                    ImGui::CloseCurrentPopup();
                }
                else {
                    // Flag to open the component exists popup
                    show_exist_popup = true;  
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