/*****************************************************************//**
 * \file   ImguiPopUps.cpp
 * \brief  imgui system
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/Imgui/sImgui.h"
#include "Core/Engine.h"

namespace NIKE
{
	bool showCreateEntityPopUp(bool pop_up)
	{
		static char entity_name[32];
		static int entity_counter_for_print = 1;
		// Popup for entity naming
		if (ImGui::BeginPopupModal("Create Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Enter a name for the new entity:");
			ImGui::InputText("##EntityNameInput", entity_name, IM_ARRAYSIZE(entity_name));

			if (ImGui::Button("OK") || NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_ENTER)) {
				// Create entity function call
				Entity::Type new_id = NIKE_ECS_MANAGER->createEntity();
				// If empty string, assign default string
				if (entity_name[0] == '\0')
				{
					snprintf(entity_name, sizeof(entity_name), "entity_%04d", entity_counter_for_print++);
				}
				// Save entity_name string
				NIKE_IMGUI_SERVICE->addEntityRef(entity_name, new_id);

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
            auto manager = NIKE_ECS_MANAGER;

            // Iterate over all registered components
            for (const auto& component : manager->getAllComponentTypes()) {
                // Component.first is the string name of the component
                // Component.second is the actual component type

                // Display each component as a button
                if (ImGui::Button(component.first.c_str())) {
                    // Check if the entity already has this component
                    if (!manager->checkEntityComponent(entity, component.second)) {
                        // Add the default-constructed component to the entity
                        manager->addDefEntityComponent(entity, component.second);

                        // Close the popup after adding the component
                        ImGui::CloseCurrentPopup();
                    }
                    else {
                        // Set flag to open the "Component Exists" popup
                        show_exist_popup = true;
                    }
                }
            }

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                show_exist_popup = false;
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

    bool showLoadAssetPopup(const std::string& asset_name) 
    {
        static bool load_asset = false;

        // Open and display the popup modal
        if (ImGui::BeginPopupModal("Load Asset", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Do you want to load this asset?");
            ImGui::Text("%s", asset_name.c_str());
            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(120, 0))) {
                // Set the flag to indicate the asset should be loaded
                load_asset = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(120, 0))) {
                // Reset the flag
                load_asset = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        bool result = load_asset;
        // Reset for next usage
        load_asset = false;
        return result;
    }

    bool removeEntityPopup() {

        static char entity_name[32] = ""; 

        // Track if the invalid entity popup should be shown
        static bool show_invalid_entity_popup = false; 

        // Track if the popup is open
        bool is_popup_open = false; 

        // Check if the popup should be opened (it will be opened if the button was clicked)
        if (ImGui::BeginPopupModal("Remove Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Enter the name of the entity to remove:");
            ImGui::InputText("##Entity Name", entity_name, IM_ARRAYSIZE(entity_name));

            // Button to confirm removal
            if (ImGui::Button("Remove")) {
                if (NIKE_IMGUI_SERVICE->checkEntityExist(entity_name)) {
                    // Remove the entity
                    NIKE_ECS_MANAGER->destroyEntity(NIKE_IMGUI_SERVICE->getEntityByName(entity_name));
                    NIKE_IMGUI_SERVICE->getEntityRef().erase(entity_name);
                    NIKE_IMGUI_SERVICE->getSelectedEntityName() = {};
                    // Reset entity_name for the next use
                    memset(entity_name, 0, sizeof(entity_name));
                    ImGui::CloseCurrentPopup(); 
                    is_popup_open = false; 
                }
                else {
                    // Show the invalid entity popup
                    show_invalid_entity_popup = true;
                    memset(entity_name, 0, sizeof(entity_name));
                    ImGui::CloseCurrentPopup();
                }

            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                memset(entity_name, 0, sizeof(entity_name));
                ImGui::CloseCurrentPopup(); 
                is_popup_open = false; 
            }

            ImGui::EndPopup();
        }
        if (show_invalid_entity_popup) {
            ImGui::OpenPopup("Invalid Entity");
            show_invalid_entity_popup = showInvalidEntityPopup();
        }
        else {
            is_popup_open = true; 
        }




        return is_popup_open;
    }

    bool showInvalidEntityPopup() {
        // Return false when the popup is closed
        bool is_open = true; 

        if (ImGui::BeginPopupModal("Invalid Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Entity does not exist.");
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
                // Close the popup
                is_open = false; 
            }
            ImGui::EndPopup();
        }

        return is_open;
    }

    // Function to display an error popup if input is invalid
    bool ShowErrorPopup() {
        bool is_open = true;
        if (ImGui::BeginPopupModal("INVALID INPUT", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", "Invalid input, please re-type your input");
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
                is_open = false;
            }
            ImGui::EndPopup();
        }
        return is_open;
    }

    // Function to display a save confirmation popup
    bool ShowSaveConfirmationPopup() {
        bool is_open = true;
        if (ImGui::BeginPopupModal("VALID INPUT", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", "Input has been saved");
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
                is_open = false;
            }
            ImGui::EndPopup();
        }
        return is_open;
    }

}