#include "Core/stdafx.h"
#include "Managers/Services/Imgui/sImgui.h"
#include "Managers/Services/Imgui/ImguiWindows.h"
#include "Core/Engine.h"
#include "../headers/Systems/Render/sysRender.h"

namespace NIKE {

	void imguiFileSystemWindow() {
		// Check if the path exists
		if (!std::filesystem::exists(GET_ASSETS_PATH())) {
			ImGui::Text("Assets folder does not exist.");
			return;
		}

		static bool show_load_popup = false;
		static std::string selected_asset;


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
				std::string file_name = path.filename().string();

				// If directory, make clickable
				if (std::filesystem::is_directory(path)) {
					if (ImGui::Selectable((file_name + "/").c_str(), false)) {
						// Update to the new path when clicking a directory
						SET_ASSETS_PATH(path);
						GET_ASSETS_PATH() = path;
					}
				}
				// If file with a valid extension, display it
				else if (hasValidTextureExtension(path)) {
					// If click on the asset, able to load it
					if (ImGui::Selectable(file_name.c_str())) {
						// Trigger the popup when selected
						selected_asset = file_name;
						show_load_popup = true;
					}
					if (show_load_popup) {
						ImGui::OpenPopup("Load Asset");
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
				ImGui::Separator();
				if (sys_percentages.empty()) {
					ImGui::Text("No active systems to report on.");
				}
				else {
					ImGui::Text("System Performance (Percentage of total game loop time):");
					ImGui::Spacing();
					for (const auto& [name, percentage] : sys_percentages) {
						double barPercent = percentage / 100.0;
						ImGui::Text("%s : %.2f%%", name.c_str(), percentage);
						ImGui::ProgressBar(barPercent, ImVec2(-1, 0));
					}
					ImGui::Spacing();
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

		ImGui::SameLine();

		// Button to remove an entity, which triggers the popup
		if (ImGui::Button("Remove Entity")) {
			open_popup = true;
			ImGui::OpenPopup("Remove Entity");
		}

		// Get the selected entity name and retrieve the corresponding entity
		std::string selected_entity_name = NIKE_IMGUI_SERVICE->getSelectedEntityName();
		//if (!selected_entity_name.empty()) {
			// Display the Remove Entity Popup if open, passing in the selected entity
			//Entity::Type to_remove = NIKEEngine.getService<IMGUI::Service>()->getEntityByName(selected_entity_name);
			open_popup = removeEntityPopup();
		//}

		// Show number of entities in the level
		ImGui::Text("Number of entities in level: %d", NIKE_ECS_MANAGER->getEntitiesCount());

		// Show entities ref name that are created
		for (auto const& elem : NIKE_IMGUI_SERVICE->getEntityRef())
		{
			if (ImGui::Selectable(elem.first.c_str(), NIKE_IMGUI_SERVICE->getSelectedEntityName() == elem.first)
				&& NIKE_IMGUI_SERVICE->checkEntityExist(elem.first))
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
		static bool show_error_popup = false;
		static bool show_save_popup = false;

		std::string selected_entity = NIKE_IMGUI_SERVICE->getSelectedEntityName();
		ImGui::Begin("Entity Component Management");

		// Check if an entity is selected
		if (!selected_entity.empty()) {
			ImGui::Text("Selected Entity: %s", selected_entity.c_str());

			Entity::Type entity = NIKE_IMGUI_SERVICE->getEntityByName(selected_entity);

			ImGui::Text("Number of Components in entity: %d", NIKE_ECS_MANAGER->getEntityComponentCount(entity));

			// Add a new component
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

			// Retrieve and display all registered component types
			for (const auto& elem : NIKE_ECS_MANAGER->getAllComponentTypes()) {
				const std::string& component_name = elem.first;
				Component::Type component_type = elem.second;

				// Check if the component exists in the entity
				if (NIKE_ECS_MANAGER->checkEntityComponent(entity, component_type) && NIKE_IMGUI_SERVICE->checkEntityExist(selected_entity)) {
					// Create a collapsible header for the component
					if (ImGui::CollapsingHeader(component_name.c_str(), ImGuiTreeNodeFlags_None)) {
						if (component_name == "Transform::Transform") {
							auto& transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
							ImGui::DragFloat2("Position", &transform_comp.position.x, 0.1f);
							ImGui::DragFloat2("Scale", &transform_comp.scale.x, 0.1f);
							ImGui::SliderFloat("Rotation", &transform_comp.rotation, -360.f, 360.f, "%.2f deg");
							// Remove Component 
							if (ImGui::Button((std::string("Remove Component##") + component_name).c_str()))
							{
								NIKE_ECS_MANAGER->removeEntityComponent(entity, component_type);
							}
						}
						else if (component_name == "Render::Texture") {
							auto& texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
							static char texture_ref[300];
							// Ensure null-termination
							texture_ref[sizeof(texture_ref) - 1] = '\0';

							ImGui::Text("Enter a texture ref:");
							if (ImGui::InputText("##textureRef", texture_ref, IM_ARRAYSIZE(texture_ref))) {}
							// Save button to confirm changes 
							if (ImGui::Button("Save Texture ID")) {
								if (NIKE_ASSETS_SERVICE->checkTextureLoaded(texture_ref))
								{
									// Update audio ID in component
									texture_comp.texture_ref = texture_ref;
									ImGui::OpenPopup("VALID INPUT");
									show_save_popup = true;
								}
								else {
									ImGui::OpenPopup("INVALID INPUT");
									show_error_popup = true;
								}
							}
							// Show pop ups
							show_error_popup = ShowErrorPopup();
							show_save_popup = ShowSaveConfirmationPopup();
							// Remove Component 
							if (ImGui::Button((std::string("Remove Component##") + component_name).c_str()))
							{
								NIKE_ECS_MANAGER->removeEntityComponent(entity, component_type);
							}
						}
						else if (component_name == "Audio::SFX") {
							auto& sfx_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
							static char input_audio_id[300];
							static char input_channel_group_id[300];
							// Ensure null-termination
							input_audio_id[sizeof(input_audio_id) - 1] = '\0';
							input_channel_group_id[sizeof(input_channel_group_id) - 1] = '\0';

							ImGui::Text("Enter Audio ID:");
							if (ImGui::InputText("##audioID", input_audio_id, IM_ARRAYSIZE(input_audio_id))) {
								// Optionally handle input change here if needed
							}

							ImGui::Text("Enter Channel ID:");
							if (ImGui::InputText("##channelID", input_channel_group_id, IM_ARRAYSIZE(input_channel_group_id))) {
								// Optionally handle input change here if needed
							}

							// Set volume
							ImGui::SliderFloat("Volume", &sfx_comp.volume, 0.f, 1.f, "%.2f");
							// Set pitch
							ImGui::SliderFloat("Pitch", &sfx_comp.pitch, 0.f, 1.f, "%.2f");

							// A button to play SFX
							if (!sfx_comp.audio_id.empty() && !sfx_comp.channel_group_id.empty())
							{
								ImGui::Text("Play Sound");
								if (ImGui::Button("Play")) {
									sfx_comp.play_sfx = !sfx_comp.play_sfx;
								}
							}


							// Save button to confirm changes to audio component
							if (ImGui::Button("Save Channel Group ID")) {
								if (NIKE_AUDIO_SERVICE->checkChannelGroupExist(input_channel_group_id))
								{
									// Update channel ID in component
									sfx_comp.channel_group_id = input_channel_group_id;
									ImGui::OpenPopup("VALID INPUT");
									show_save_popup = true;
								}
								else
								{
									ImGui::OpenPopup("INVALID INPUT");
									show_error_popup = true;
								}
							}
							if (ImGui::Button("Save Audio ID")) {
								if (NIKE_ASSETS_SERVICE->checkAudioExist(input_audio_id))
								{
									// Update audio ID in component
									sfx_comp.audio_id = input_audio_id;
									ImGui::OpenPopup("VALID INPUT");
									show_save_popup = true;
								}
								else
								{
									ImGui::OpenPopup("INVALID INPUT");
									show_error_popup = true;
								}
							}

							// Show pop ups
							show_error_popup = ShowErrorPopup();
							show_save_popup = ShowSaveConfirmationPopup();

							// Remove Component 
							if (ImGui::Button((std::string("Remove Component##") + component_name).c_str()))
							{
								NIKE_ECS_MANAGER->removeEntityComponent(entity, component_type);
							}
						}
						else
						{
							// Other components that do not need specfic UI goes here
							// Remove Component 
							if (ImGui::Button((std::string("Remove Component##") + component_name).c_str()))
							{
								NIKE_ECS_MANAGER->removeEntityComponent(entity, component_type);
							}
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

		ImGui::End();
	}
	void imguiShowLoadedAssetsWindow()
	{
		ImGui::Begin("Loaded Assets");

		// Display all loaded textures
		for (const auto& texture : NIKE_ASSETS_SERVICE->getLoadedTextures())
		{
			ImGui::Text("%s", texture.first.c_str());
		}

		ImGui::End();
	}

	void imguiShowGameViewport()
	{
		ImGui::Begin("Game Viewport");
		ImTextureID textureID = (ImTextureID)NIKE_ECS_MANAGER->getSystemInstance<Render::Manager>()->getTextureColorBuffer();
		// Define UV coordinates to flip the texture vertically
		ImVec2 uv0(0.0f, 1.0f); // Bottom-left
		ImVec2 uv1(1.0f, 0.0f); // Top-right

		ImGui::Image(textureID, ImVec2(1024, 576), uv0, uv1);
		ImGui::End();
	}

	void imguiCameraControl()
	{
		ImGui::Begin("Camera Control");

		// Position Controls
		ImGui::Text("Position:");
		// Create a grid of buttons for up, down, left, and right
		if (ImGui::Button("Up")) {
			// Move camera position up
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::UP));
		}
		if (ImGui::IsItemActive()) {
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::UP));
		}


		ImGui::SameLine();
		if (ImGui::Button("Down")) {
			// Move camera position down
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::DOWN));
		}
		if (ImGui::IsItemActive()) {
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::DOWN));
		}


		ImGui::SameLine();
		if (ImGui::Button("Left")) {
			// Move camera position left
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::LEFT));
		}
		if (ImGui::IsItemActive()) {
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::LEFT));
		}


		ImGui::SameLine();
		if (ImGui::Button("Right")) {
			// Move camera position right
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::RIGHT));
		}
		if (ImGui::IsItemActive()) {
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::RIGHT));
		}

		if (ImGui::Button("Reset Position")) {
			// Move camera position right

			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::RESET_POS));

		}

		// Zoom Controls
		ImGui::Text("Zoom:");
		if (ImGui::Button("Zoom In")) {
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::NONE, NIKE::Render::CamZoom::ZOOM_IN));
		}
		if (ImGui::IsItemActive()) {
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::NONE, NIKE::Render::CamZoom::ZOOM_IN));
		}

		ImGui::SameLine();
		if (ImGui::Button("Zoom Out")) {
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::NONE, NIKE::Render::CamZoom::ZOOM_OUT));
		}
		if (ImGui::IsItemActive()) {
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::NONE, NIKE::Render::CamZoom::ZOOM_OUT));
		}

		if (ImGui::Button("Reset Cam")) {
			// Move camera position right

			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::NONE, NIKE::Render::CamZoom::RESET_ZOOM));

		}


		ImGui::End();
	}
}

