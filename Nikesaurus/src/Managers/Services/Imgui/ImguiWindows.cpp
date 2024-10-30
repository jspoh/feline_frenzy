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

		static bool show_load_popup = false;

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
					// If click on the asset, able to load it
					if (ImGui::Selectable(filename.c_str())) {
						// Trigger the popup when selected
						show_load_popup = true;
					}
				}
			}
		}
		else {
			ImGui::Text("No valid directory or path.");
		}

		ImGui::End();
	}

	void imguiDebuggingWindow() {

		// Begin ImGui window
		ImGui::Begin("Debugging Tools");

		if (ImGui::BeginTabBar("TabBar")) {
			// Performance Viewer Tab
			if (ImGui::BeginTabItem("Performance Viewer")) {

				// Display FPS 
				float fps = ImGui::GetIO().Framerate;
				ImGui::Text("FPS: %.2f", fps);

				// Display a FPS in a graph
				static float fpsValues[100] = { 0 };
				static int fpsIndex = 0;
				fpsValues[fpsIndex] = fps;
				fpsIndex = (fpsIndex + 1) % IM_ARRAYSIZE(fpsValues);
				ImGui::PlotLines("FPS", fpsValues, IM_ARRAYSIZE(fpsValues), 0, NULL, 0.0f, 120.0f, ImVec2(0, 80));

				ImGui::Spacing();
				// Display System Usage (Data all being handled in sDebug)
				auto& sys_percentages = NIKE_DEBUG_SERVICE->system_percentages;

				if (sys_percentages.empty()) {
					ImGui::Text("No active systems to report on.");
				}
				else {
					ImGui::Text("System Performance (Percentage of total game loop time):");

					for (const auto& [name, percentage] : sys_percentages) {
						ImGui::Text("%s : %.2f%%", name.c_str(), percentage);
					}

					ImGui::Text("Total Active System Time: %.2f%%", NIKE_DEBUG_SERVICE->total_system_time);
				}

				ImGui::EndTabItem();
			}

			// Crash Logger Tab
			if (ImGui::BeginTabItem("Crash Logger")) {
				// Open crash log file
				std::ifstream crashLogFile("logs/crash-log.txt");

				if (crashLogFile.is_open()) {
					std::string line;
					std::string logs;

					while (std::getline(crashLogFile, line)) {
						logs += line + "\n";
					}
					crashLogFile.close();

					ImGui::BeginChild("CrashLogScrollArea", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::TextUnformatted(logs.c_str());
					ImGui::EndChild();
				}
				else {
					ImGui::Text("Crash log file could not be found!");
				}
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
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
		ImGui::Text("Number of entities in level: %d", NIKE_ECS_MANAGER->getEntitiesCount());

		// Show entities ref name that are created
		for (auto const& elem : NIKE_IMGUI_SERVICE->getEntityRef())
		{
			if (ImGui::Selectable(elem.first.c_str(), NIKE_IMGUI_SERVICE->getSelectedEntityName() == elem.first))
			{
				// Update the selected entity name
				NIKE_IMGUI_SERVICE->getSelectedEntityName() = elem.first;
			}
		}

		ImGui::End();
	}

	void imguiEntityComponentManagementWindow()
	{
		static bool open_component_popup = false;

		std::string selected_entity = NIKE_IMGUI_SERVICE->getSelectedEntityName();
		ImGui::Begin("Entity Component Management");

		// Check if an entity is selected
		if (!selected_entity.empty()) {
			ImGui::Text("Selected Entity: %s", selected_entity.c_str());

			Entity::Type entity = NIKE_IMGUI_SERVICE->getEntityByName(selected_entity);
			ImGui::Text("Number of Components in entity: %d", NIKE_ECS_MANAGER->getEntityComponentCount(entity));

			// Option to add a new component
			if (ImGui::Button("Add Component")) {
				open_component_popup = true;
				ImGui::OpenPopup("Add Component");
			}

			// Show add component popup
			open_component_popup = showAddComponentPopup(entity, open_component_popup);

			ImGui::SameLine();

			if (ImGui::Button("Clone Entity")) {
				// TODO: Create popup to choose which entity to clone
			}

			auto coordinator_manager = NIKE_ECS_MANAGER;

			// Retrieve and display all registered component types
			for (const auto& elem : coordinator_manager->getAllComponentTypes()) {
				const std::string& component_name = elem.first;
				Component::Type component_type = elem.second;

				// Check if the component exists in the entity
				if (coordinator_manager->checkEntityComponent(entity, component_type)) {
					// Create a collapsible header for the component
					if (ImGui::CollapsingHeader(component_name.c_str(), ImGuiTreeNodeFlags_None)) {
						if (component_name == "Transform::Transform") {
							auto& transform_comp = coordinator_manager->getEntityComponent<Transform::Transform>(entity);
							ImGui::DragFloat2("Position", &transform_comp.position.x, 0.1f);
							ImGui::DragFloat2("Scale", &transform_comp.scale.x, 0.1f);
							ImGui::SliderAngle("Rotation", &transform_comp.rotation);
						}
						else if (component_name == "Render::Texture") {
							auto& texture_comp = coordinator_manager->getEntityComponent<Render::Texture>(entity);
							static char texture_ref[300] = {};
							ImGui::Text("Enter a texture ref:");
							ImGui::InputText("##textureRef", texture_ref, IM_ARRAYSIZE(texture_ref));
						}
						// Additional component-specific UI elements can go here
					}
				}
			}
		}
		else {
			ImGui::Text("No entity selected.");
		}

		ImGui::End();
	}


	void imguiRenderEntityWindow()
	{
		ImGui::Begin("Levels Management");

		unsigned int tex_id = NIKE_ASSETS_SERVICE->getTexture("test")->gl_data;
		Vector2i tex_size = NIKE_ASSETS_SERVICE->getTexture("test")->size;

		ImGui::Image((intptr_t)tex_id, ImVec2((float)tex_size.x / 2, (float)tex_size.y / 2));

		ImGui::End();
	}
	void imguiShowLoadedAssetsWindow()
	{
		ImGui::Begin("Loaded Assets");

		ImGui::End();
	}
}

