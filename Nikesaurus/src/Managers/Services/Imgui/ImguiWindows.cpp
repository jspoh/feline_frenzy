#include "Core/stdafx.h"
#include "Managers/Services/Imgui/sImgui.h"
#include "Managers/Services/Imgui/ImguiWindows.h"
#include "Core/Engine.h"

namespace NIKESAURUS {

	void imguiFileSystemWindow() {
		// Check if the path exists
		if (!std::filesystem::exists(GET_ASSETS_PATH())) {
			ImGui::Text("Assets folder does not exist.");
			return;
		}

		// Begin the ImGui window with a title
		ImGui::Begin("Assets Browser");

		// Display the current path
		ImGui::Text("Current Path: %s", GET_ASSETS_PATH().string().c_str());

		// Navigate back to the root directory
		std::filesystem::path const root_assets_path = "C:\\Users\\User\\feline_frenzy\\Feline Frenzy\\assets";
		if (GET_ASSETS_PATH() != root_assets_path) {
			if (ImGui::Button("Back")) {
				// Go up one directory level
				SET_ASSETS_PATH(GET_ASSETS_PATH().parent_path());
				GET_ASSETS_PATH() = GET_ASSETS_PATH().parent_path();
			}
		}

		// List directories and files
		if (std::filesystem::is_directory(GET_ASSETS_PATH())) {
			for (const auto& entry : std::filesystem::directory_iterator(GET_ASSETS_PATH())) {
				const std::filesystem::path& path = entry.path();
				std::string filename = path.filename().string();

				// If directory, make clickable
				if (std::filesystem::is_directory(path)) {
					if (ImGui::Selectable((filename + "/").c_str(), false)) {
						// Update to the new path when clicking a directory
						SET_ASSETS_PATH(path);
						GET_ASSETS_PATH() = path; 
					}
				}
				// If file with a valid extension, display it
				else if (hasValidExtension(path)) {
					ImGui::Text("%s", filename.c_str());
				}
			}
		}
		else {
			ImGui::Text("No valid directory or path.");
		}

		ImGui::End();
	}

	void imguiEntityWindow()
	{	
		static bool open_popup = false;

		// Begin the ImGui window
		ImGui::Begin("Entities Window");

		// Button to create an entity, which triggers the popup
		if (ImGui::Button("Create Entity")) {
			open_popup = true;
			ImGui::OpenPopup("Create Entity");
		}

		// Display the Create Entity Popup if open
		open_popup = showCreateEntityPopUp(open_popup);

		// Show number of entities in the level
		ImGui::Text("Number of entities in level: %d", NIKEEngine.getService<Coordinator::Service>()->getEntitiesCount());

		// Show entities ref name that are created
		for (auto const& elem : NIKEEngine.getService<IMGUI::Service>()->getEntityRef())
		{
			if (ImGui::Selectable(elem.first.c_str(), NIKEEngine.getService<IMGUI::Service>()->getSelectedEntityName() == elem.first))
			{
				// Update the selected entity name
				NIKEEngine.getService<IMGUI::Service>()->getSelectedEntityName() = elem.first;
			}
		}
		
		ImGui::End();
	}

	void imguiEntityComponentManagementWindow()
	{
		static bool open_component_popup = false;

		std::string selected_entity = NIKEEngine.getService<IMGUI::Service>()->getSelectedEntityName();
		ImGui::Begin("Entity Component Management");

		// Check if an entity is selected
		if (!selected_entity.empty()) {
			ImGui::Text("Selected Entity: %s", selected_entity.c_str());

			Entity::Type entity = NIKEEngine.getService<IMGUI::Service>()->getEntityByName(selected_entity);

			//// For easier typing
			//auto component_manager = NIKEEngine.getService<Component::Manager>();

			//// Retrieve all registered component types
			//for (const auto& elem : NIKEEngine.getService<Component::Manager>()->getComponentTypes()) {
			//	// Check if the component type exists for the selected entity
			//	if (NIKEEngine.getService<Coordinator::Manager>()->checkEntityComponent<std::any>(entity)) {
			//		// Retrieve and display the component using ImGui
			//		auto& component = NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<std::any>(entity);
			//		ImGui::Text("Component: %s", elem.first.c_str());
			//	}
			//}

			// Option to add a new component
			if (ImGui::Button("Add Component")) {
				// Logic to add a component to the entity
				open_component_popup = true;
				ImGui::OpenPopup("Add Component");
			}

			ImGui::SameLine();

			if (ImGui::Button("Clone Entity")) {
				// Clone entity logic
				// TODO: Create pop up to choose which entity to clone
				// NIKEEngine.getService<Coordinator::Service>()->cloneEntity();
			}

			open_component_popup = showAddComponentPopup(entity, open_component_popup);
			
			
		}
		else {
			ImGui::Text("No entity selected.");
		}

		
		ImGui::End();
	}
}

