/*****************************************************************//**
 * \file   ImguiUtils.cpp
 * \brief  imgui system
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (60%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (40%)
 * \Brief This file contains declarations of utility functions to be used
 * \date   September 2024
 * All content ? 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/Imgui/sImgui.h"
#include "Managers/Services/Imgui/ImguiWindows.h"
#include "Core/Engine.h"
#include "../headers/Systems/Render/sysRender.h"

namespace NIKE {

	void imguiDockingSpace() {
		// Main DockSpace
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x),
			static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y)));
		window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;



		// Create a full-screen docking space
		ImGui::Begin("Level Editor", nullptr, window_flags);
		bool is_paused = NIKE_IMGUI_SERVICE->getGamePaused();
		bool is_debug = NIKE_IMGUI_SERVICE->getDebugMode();

		// Menu Bar for Play/Pause controls
		if (ImGui::BeginMenuBar()) {
			ImGui::Spacing();
			ImGui::Text("Play / Pause Controls : ");
			if (ImGui::Button(is_paused ? "Play" : "Pause")) {
				NIKE_IMGUI_SERVICE->setGamePaused(!is_paused);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset Scene")) {
				std::string scene_file_path = NIKE_SERIALIZE_SERVICE->getCurrSceneFile();

				if (!scene_file_path.empty() && std::filesystem::exists(scene_file_path)) {
					NIKE_IMGUI_SERVICE->getSelectedEntityName() = "";

					// Clear previous scene entities before loading the new one
					NIKE_ECS_MANAGER->destroyAllEntities();
					NIKE_IMGUI_SERVICE->getEntityRef().clear();

					// Load the scene from the selected file path
					NIKE_SERIALIZE_SERVICE->loadSceneFromFile(scene_file_path);
					NIKE_IMGUI_SERVICE->populateLists = false;
				}
				else {

					NIKEE_CORE_ERROR("Error: Scene file path is invalid or file does not exist.");
				}
			}
			ImGui::SameLine();
			ImGui::Text("Toggle Debug Box : ");
			if (ImGui::Button(is_debug ? "Hide" : "Show")) {
				NIKE_IMGUI_SERVICE->setDebugMode(!is_debug);
			}
			ImGui::Spacing();


			ImGui::EndMenuBar();
		}
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();

	}

	//void imguiFileSystemWindow() {
	//	// Check if the path exists
	//	if (!std::filesystem::exists(GET_ASSETS_PATH())) {
	//		ImGui::Text("Assets folder does not exist.");
	//		return;
	//	}

	//	static bool show_load_popup = false;
	//	static std::string selected_asset;


	//	// Begin the ImGui window with a title
	//	ImGui::Begin("Assets Browser");

	//	// Display the current path
	//	ImGui::Text("Current Path: %s", GET_ASSETS_PATH().string().c_str());

	//	// Navigate back to the root directory
	//	std::filesystem::path const root_assets_path = "assets/";
	//	if (GET_ASSETS_PATH() != root_assets_path) {
	//		if (ImGui::Button("Back")) {
	//			// Go up one directory level
	//			SET_ASSETS_PATH(GET_ASSETS_PATH().parent_path());
	//			GET_ASSETS_PATH() = GET_ASSETS_PATH().parent_path();
	//		}
	//	}

	//	// List directories and files
	//	if (std::filesystem::is_directory(GET_ASSETS_PATH())) {
	//		for (const auto& entry : std::filesystem::directory_iterator(GET_ASSETS_PATH())) {
	//			const std::filesystem::path& path = entry.path();
	//			std::string file_name = path.filename().string();

	//			// If directory, make clickable
	//			if (std::filesystem::is_directory(path)) {
	//				if (ImGui::Selectable((file_name + "/").c_str(), false)) {
	//					// Update to the new path when clicking a directory
	//					SET_ASSETS_PATH(path);
	//					GET_ASSETS_PATH() = path;
	//				}
	//			}
	//			// Handle different asset types and drag-and-drop
	//			else if (hasValidTextureExtension(path) || hasValidAudioExtension(path) || hasValidFontExtension(path)) {
	//				if (ImGui::Selectable(file_name.c_str())) {
	//					selected_asset = file_name;
	//					show_load_popup = true;
	//				}
	//			}
	//		}
	//	}
	//	else {
	//		ImGui::Text("No valid directory or path.");
	//	}

	//	ImGui::End();
	//}

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
						ImGui::ProgressBar(static_cast<float>(barPercent), ImVec2(-1, 0));
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
		open_popup = showCreateEntityPopUp();

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
		open_popup = removeEntityPopup(selected_entity_name);
		//}

		// Show number of entities in the level
		ImGui::Text("Number of entities in level: %d", NIKE_ECS_MANAGER->getEntitiesCount());

		//Update entities list
		if (NIKE_ECS_MANAGER->getEntitiesCount() != NIKE_IMGUI_SERVICE->getEntityRef().size()) {
			for (auto const& entity : NIKE_ECS_MANAGER->getAllEntities()) {

				bool b_found = false;
				for (auto const& elem : NIKE_IMGUI_SERVICE->getEntityRef()) {
					if (elem.second == entity) {
						b_found = true;
						break;
					}
				}

				if (b_found) continue;

				static char entity_name[32];
				snprintf(entity_name, sizeof(entity_name), "entity_%04d", entity);

				//Add entity ref
				NIKE_IMGUI_SERVICE->addEntityRef(entity_name, entity);
			}
		}

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
		static bool open_save_entity_popup = false;
		static bool show_error_popup = false;
		static bool show_save_popup = false;
		static bool open_clone_popup = false;
		static bool open_layer_popup = false;

		std::string selected_entity = NIKE_IMGUI_SERVICE->getSelectedEntityName();
		ImGui::Begin("Entity Component Management");

		// Check if an entity is selected
		if (!selected_entity.empty()) {
			ImGui::Text("Selected Entity: %s", selected_entity.c_str());

			Entity::Type entity = NIKE_IMGUI_SERVICE->getEntityByName(selected_entity);

			ImGui::Text("Number of Components in entity: %d", NIKE_ECS_MANAGER->getEntityComponentCount(entity));

			ImGui::Text("Entity's Layer: %d", NIKE_ECS_MANAGER->getEntityLayerID(entity));

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
				open_clone_popup = true;
				ImGui::OpenPopup("Clone Entity");
			}

			open_clone_popup = cloneEntityPopup();

			// Add a new component
			if (ImGui::Button("Save Prefab")) {
				open_save_entity_popup = true;
				ImGui::OpenPopup("Save Prefab");
			}

			// Show save entity popup
			open_save_entity_popup = savePrefabPopup(entity);

			ImGui::SameLine();

			if (ImGui::Button("Set Layer ID")) {
				// TODO: Create popup to choose which entity to clone
				open_layer_popup = true;
				ImGui::OpenPopup("Set Layer ID");
			}

			open_layer_popup = changeLayerPopup(entity);

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
							static char texture_id[300];
							static bool texture_initialized = false;

							//For initial initialization
							if (!texture_initialized || texture_id != texture_comp.texture_id.c_str()) {
								// Ensure null-termination
								texture_id[sizeof(texture_id) - 1] = '\0';
								strcpy_s(texture_id, texture_comp.texture_id.c_str());
								texture_initialized = true;
							}

							ImGui::Text("Enter a texture ref:");
							if (ImGui::InputText("##textureRef", texture_id, IM_ARRAYSIZE(texture_id)))
							{
								texture_comp.texture_id = texture_id;
							}
							// Begin drag-and-drop target for texture reference
							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture"))
								{
									// Retrieve the texture ID from the payload and set it as the texture_id
									const char* dropped_texture = static_cast<const char*>(payload->Data);
									strncpy_s(texture_id, dropped_texture, sizeof(texture_id) - 1);
									texture_id[sizeof(texture_id) - 1] = '\0';
									texture_comp.texture_id = texture_id;
								}
								ImGui::EndDragDropTarget();
							}
							ImGui::DragFloat4("Texture Color (RBGA)", &texture_comp.color.r, 0.1f);
							ImGui::DragInt2("Frame Size", &texture_comp.frame_size.x, 1);
							ImGui::DragInt2("Frame Index", &texture_comp.frame_index.x, 1);
							ImGui::DragFloat("Intensity", &texture_comp.intensity, 0.1f);
							// Save button to confirm changes 
							if (ImGui::Button("Save Texture ID")) {
								if (NIKE_ASSETS_SERVICE->checkTextureExist(texture_id))
								{
									// Update audio ID in component
									texture_comp.texture_id = texture_id;
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
							if (!texture_comp.texture_id.empty())
							{
								ImGui::Text("Stretch: %s", texture_comp.b_stretch ? "true" : "false");
								if (ImGui::Button("Stretch")) {
									texture_comp.b_stretch = !texture_comp.b_stretch;
								}
								ImGui::Text("Blend: %s", texture_comp.b_blend ? "true" : "false");
								if (ImGui::Button("Blend")) {
									texture_comp.b_blend = !texture_comp.b_blend;
								}
								ImGui::Text("Flip Horizontally: %s", texture_comp.b_flip.x ? "true" : "false");
								if (ImGui::Button("Flip X")) {
									texture_comp.b_flip.x = !texture_comp.b_flip.x;
								}
								ImGui::Text("Flip Vertically: %s", texture_comp.b_flip.y ? "true" : "false");
								if (ImGui::Button("Flip Y")) {
									texture_comp.b_flip.y = !texture_comp.b_flip.y;
								}
							}
							// Remove Component 
							if (ImGui::Button((std::string("Remove Component##") + component_name).c_str()))
							{
								NIKE_ECS_MANAGER->removeEntityComponent(entity, component_type);
							}
						}
						else if (component_name == "Physics::Dynamics") {
							auto& dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);

							ImGui::DragFloat2("Velocity", &dynamics_comp.velocity.x, 0.1f);
							ImGui::DragFloat2("Force", &dynamics_comp.force.x, 0.1f);
							ImGui::DragFloat("Max Speed", &dynamics_comp.max_speed, 0.1f);
							ImGui::DragFloat("Drag", &dynamics_comp.drag, 0.1f);
							ImGui::DragFloat("Mass", &dynamics_comp.mass, 0.1f);

							// Remove Component 
							if (ImGui::Button((std::string("Remove Component##") + component_name).c_str()))
							{
								NIKE_ECS_MANAGER->removeEntityComponent(entity, component_type);
							}
						}
						else if (component_name == "Render::Cam") {
							auto& cam_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(entity);

							// Default value for cam height will be the window height if there is no adjustments
							if (cam_comp.height <= 0.0f) {
								cam_comp.height = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);
							}

							// Don't add position
							//ImGui::DragFloat2("Position", &cam_comp.position.x, 0.1f);
							ImGui::DragFloat("Height", &cam_comp.height, 0.1f);

							// Remove Component 
							if (ImGui::Button((std::string("Remove Component##") + component_name).c_str()))
							{
								NIKE_ECS_MANAGER->removeEntityComponent(entity, component_type);
								NIKE_IMGUI_SERVICE->populateLists = false;
							}
						}
						else if (component_name == "Animation::Base") {
							auto& animate_base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);

							ImGui::DragInt("Number of Animations (If set to 0, infinite number of animations)", &animate_base_comp.animations_to_complete, 1);
							ImGui::DragFloat("Frame Duration", &animate_base_comp.frame_duration, .1f);
							ImGui::Text("Animation timer: %f", &animate_base_comp.timer);
							ImGui::Text("Completed Animations: %d", animate_base_comp.completed_animations);

							// Variable to hold the selected resolution
							static NIKE::Animation::Mode selected_mode = NIKE::Animation::Mode::PLAYING;
							const char* mode_names[] = { "PLAYING", "PAUSE", "RESTART", "END" };
							int current_mode = static_cast<int>(selected_mode);

							//Current active mode
							current_mode = static_cast<int>(animate_base_comp.animation_mode);

							// Display the selected resolution
							ImGui::Text("Current Origin: %s", mode_names[current_mode]);

							// Combo box for selecting text origin
							if (ImGui::Combo("##Origin", &current_mode, mode_names, IM_ARRAYSIZE(mode_names))) {
								selected_mode = static_cast<NIKE::Animation::Mode>(current_mode);
								animate_base_comp.animation_mode = selected_mode;
							}

							//ImGui::Text("Ping Pong? %s", animate_base_comp.b_pingpong ? "yes" : "no");
							//if (ImGui::Button("Toggle Ping Pong")) {
							//	animate_base_comp.b_pingpong = !animate_base_comp.b_pingpong;
							//}

							ImGui::Text("Reverse Animation? %s", animate_base_comp.b_reverse ? "yes" : "no");
							if (ImGui::Button("Toggle Reverse")) {
								animate_base_comp.b_reverse = !animate_base_comp.b_reverse;
							}

							// Remove Component 
							if (ImGui::Button((std::string("Remove Component##") + component_name).c_str()))
							{
								NIKE_ECS_MANAGER->removeEntityComponent(entity, component_type);
							}
						}
						else if (component_name == "Animation::Sprite") {
							auto& animate_sprite_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);

							ImGui::DragInt2("Sheet Size", &animate_sprite_comp.sheet_size.x, 1);
							ImGui::DragInt2("Start Index", &animate_sprite_comp.start_index.x, 1);
							ImGui::DragInt2("End Index", &animate_sprite_comp.end_index.x, 1);
							ImGui::Text("Current Index:");
							ImGui::BulletText("X = %d", animate_sprite_comp.curr_index.x);
							ImGui::BulletText("Y = %d", animate_sprite_comp.curr_index.y);

							// Remove Component 
							if (ImGui::Button((std::string("Remove Component##") + component_name).c_str()))
							{
								NIKE_ECS_MANAGER->removeEntityComponent(entity, component_type);
							}
						}
						else if (component_name == "Render::Shape") {
							auto& shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);
							static char input_model_ref[300];
							static bool shape_initialized = false;

							if (!shape_initialized) {
								// Ensure null-termination
								input_model_ref[sizeof(input_model_ref) - 1] = '\0';
								strcpy_s(input_model_ref, shape_comp.model_id.c_str());
								shape_initialized = true;
							}

							ImGui::Text("Enter the shape model to use:");
							if (ImGui::InputText("##shapeRef", input_model_ref, IM_ARRAYSIZE(input_model_ref))) {
								// Optionally handle input change here if needed
							}
							ImGui::DragFloat4("Color in RBGA", &shape_comp.color.r, 0.1f);
							// Save buttons to confirm changes
							if (ImGui::Button("Save Font Ref")) {
								if (NIKE_ASSETS_SERVICE->checkModelExist(input_model_ref))
								{
									// Update channel ID in component
									shape_comp.model_id = input_model_ref;
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
						else if (component_name == "Render::Text") {
							auto& text_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(entity);

							static char input_font_id[300];
							static char input_text[300];
							static bool text_initialized = false;

							if (!text_initialized) {
								// Ensure null-termination
								input_font_id[sizeof(input_font_id) - 1] = '\0';
								input_text[sizeof(input_text) - 1] = '\0';
								strcpy_s(input_font_id, text_comp.font_id.c_str());
								strcpy_s(input_text, text_comp.text.c_str());
								text_initialized = true;
							}

							ImGui::Text("Enter the font to use:");
							if (ImGui::InputText("##fontRef", input_font_id, IM_ARRAYSIZE(input_font_id))) {
								// Optionally handle input change here if needed
							}

							ImGui::Text("Enter text to display:");
							if (ImGui::InputText("##text", input_text, IM_ARRAYSIZE(input_text))) {
								// Stuff
							}

							ImGui::DragFloat4("Text Color (RBGA)", &text_comp.color.r, 0.1f);
							ImGui::DragFloat("Text Scale", &text_comp.scale, 0.02f);
							text_comp.scale = std::clamp(text_comp.scale, EPSILON, 10.0f);
							ImGui::Text((std::string("Text Size X: ") + std::to_string(text_comp.size.x)).c_str());
							ImGui::Text((std::string("Text Size Y: ") + std::to_string(text_comp.size.y)).c_str());

							// Variable to hold the selected resolution
							static NIKE::Render::TextOrigin selected_origin = NIKE::Render::TextOrigin::CENTER;

							// Display a dropdown to select resolution
							// For now this hard code it 
							const char* origin_names[] = { "CENTER", "BOTTOM", "TOP", "LEFT", "RIGHT" };
							int current_origin = static_cast<int>(selected_origin);

							// Display the selected text origin
							ImGui::Text("Current Text Origin: %s", origin_names[current_origin]);

							// Combo box for selecting text origin
							if (ImGui::Combo("##Origin", &current_origin, origin_names, IM_ARRAYSIZE(origin_names))) {
								selected_origin = static_cast<NIKE::Render::TextOrigin>(current_origin);
								text_comp.origin = selected_origin;
							}


							// Save buttons to confirm changes
							if (ImGui::Button("Save Font Ref")) {
								if (NIKE_ASSETS_SERVICE->checkFontExist(input_font_id))
								{
									// Update channel ID in component
									text_comp.font_id = input_font_id;
									ImGui::OpenPopup("VALID INPUT");
									show_save_popup = true;
								}
								else
								{
									ImGui::OpenPopup("INVALID INPUT");
									show_error_popup = true;
								}
							}

							if (ImGui::Button("Save Text to show")) {
								text_comp.text = input_text;
								ImGui::OpenPopup("VALID INPUT");
								show_save_popup = true;
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
						else if (component_name == "Physics::Collider") {
							auto& dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);

							// Variable to hold the selected resolution
							static NIKE::Physics::Resolution selected_resolution = NIKE::Physics::Resolution::NONE;

							// Display a dropdown to select resolution
							// For now this hard code it 
							const char* resolution_names[] = { "NONE", "SLIDE", "BOUNCE" };
							int current_resolution = static_cast<int>(selected_resolution);

							//Update resolution
							current_resolution = static_cast<int>(dynamics_comp.resolution);

							// Display the selected resolution
							ImGui::Text("Current Resolution: %s", resolution_names[current_resolution]);

							// Combo box for selecting resolution
							if (ImGui::Combo("##Resolution", &current_resolution, resolution_names, IM_ARRAYSIZE(resolution_names))) {
								selected_resolution = static_cast<NIKE::Physics::Resolution>(current_resolution);
								dynamics_comp.resolution = selected_resolution;
							}



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
							static bool audio_initialized = false;

							if (!audio_initialized) {
								// Ensure null-termination
								input_audio_id[sizeof(input_audio_id) - 1] = '\0';
								input_channel_group_id[sizeof(input_channel_group_id) - 1] = '\0';
								strcpy_s(input_audio_id, sfx_comp.audio_id.c_str());
								strcpy_s(input_channel_group_id, sfx_comp.channel_group_id.c_str());
								audio_initialized = true;
							}

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
									sfx_comp.b_play_sfx = !sfx_comp.b_play_sfx;
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
								NIKE_IMGUI_SERVICE->populateLists = false;
								NIKE_ECS_MANAGER->removeEntityComponent(entity, component_type);
							}
						}
						else if (component_name == "GameLogic::Player") {
							auto& e_player = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::Player>(entity);

							static char script_path[300];
							static bool scripth_path_init = false;

							////For initial initialization
							//if (!scripth_path_init || script_path != e_player.script.c_str()) {
							//	// Ensure null-termination
							//	script_path[sizeof(script_path) - 1] = '\0';
							//	strcpy_s(script_path, e_player.script.c_str());
							//	scripth_path_init = true;
							//}

							ImGui::Text("Enter a script:");
							if (ImGui::InputText("##PlayerScript", script_path, IM_ARRAYSIZE(script_path))){

							}

							if (ImGui::Button("Save Script")) {
								e_player.script = script_path;
							}

							ImGui::Separator();

							ImGui::Text(std::string("Script Id: " + e_player.script_id).c_str());
						}
						else
						{
							// Other components that do not need specfic UI goes here
							// Remove Component 
							if (ImGui::Button((std::string("Remove Component##") + component_name).c_str()))
							{
								NIKE_IMGUI_SERVICE->populateLists = false;
								NIKE_ECS_MANAGER->removeEntityComponent(entity, component_type);
							}
						}
						// Additional component-specific UI elements goes here
					}
				}
			}
		}
		else {
			ImGui::Text("No entity selected.");
		}

		ImGui::End();
	}

	void imguiShowLoadedAssetsWindow()
	{
		ImGui::Begin("Asset List");

		// Tabs for different asset types
		if (ImGui::BeginTabBar("Asset Types"))
		{
			// Levels tab for .scn files
			if (ImGui::BeginTabItem("Prefabs"))
			{
				displayAssetList("Prefabs");
				ImGui::EndTabItem();
			}

			// Textures tab
			if (ImGui::BeginTabItem("Textures"))
			{
				displayAssetList("Textures");
				ImGui::EndTabItem();
			}

			// Models tab
			//if (ImGui::BeginTabItem("Models"))
			//{
			//	displayAssetList("Models");
			//	ImGui::EndTabItem();
			//}

			// Font tab
			if (ImGui::BeginTabItem("Fonts"))
			{
				displayAssetList("Fonts");
				ImGui::EndTabItem();
			}

			// Shaders tab
			//if (ImGui::BeginTabItem("Shaders"))
			//{
			//	displayAssetList("Shaders");
			//	ImGui::EndTabItem();
			//}

			// Audio tab
			if (ImGui::BeginTabItem("Audio"))
			{
				displayAssetList("Audio");
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}

	void imguiShowLoadedLevelsWindow()
	{
		ImGui::Begin("Levels");

		// Always show "Save Scene" button
		if (ImGui::Button("Save Scene"))
		{
			ImGui::OpenPopup("Save Scene As");
		}

		// Tabs for different asset types
		//if (ImGui::BeginTabBar("Asset Types"))
		//{
			// Levels tab for .scn files
		//	if (ImGui::BeginTabItem("Levels"))
		//	{
		displayAssetList("Levels");
		//		ImGui::EndTabItem();
		//	}
		//}

		// Show the "Save Scene As" popup if the button was clicked
		if (ImGui::BeginPopupModal("Save Scene As"))
		{
			static char file_input[128] = "";
			ImGui::InputText("Filename", file_input, IM_ARRAYSIZE(file_input));

			if (ImGui::Button("Save"))
			{
				std::string scene_name = file_input;
				if (scene_name.empty())
				{
					scene_name = "default";
				}

				std::string file_path = NIKE_ASSETS_SERVICE->getScenesPath() + scene_name + ".scn";
				std::filesystem::directory_entry scene_file_path(file_path);
				NIKE_SERIALIZE_SERVICE->saveSceneToFile(file_path);
				NIKE_ASSETS_SERVICE->loadScn(scene_file_path);
				// Reset for the next use
				memset(file_input, 0, sizeof(file_input));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				memset(file_input, 0, sizeof(file_input));
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void imguiLayerManagementWindow()
	{
		ImGui::Begin("Layer Management", nullptr, ImGuiWindowFlags_NoResize);

		// Static variables for managing the selected layer and bit manipulations
		static unsigned int edit_mask_id = 0;
		static bool bit_state = false;
		static unsigned int selected_layer_index = 0;
		static unsigned int bit_position = 0;

		unsigned int layer_count = NIKE_SCENES_SERVICE->getCurrScene()->getLayerCount();
		const auto& layers = NIKE_SCENES_SERVICE->getCurrScene()->getLayers();

		std::vector<std::string> layer_names;
		for (int i = 0; i < layers.size(); ++i) {
			layer_names.push_back("Layer " + std::to_string(NIKE_SCENES_SERVICE->getCurrScene()->getLayer(i)->getLayerID()));
		}

		// Display layer count
		ImGui::Text("Total Layers: %u", layer_count);

		// Layer selection dropdown
		if (!layers.empty()) {
			// Show dropdown to select a layer
			if (ImGui::BeginCombo("Select Layer", (selected_layer_index >= 0 ? layer_names[selected_layer_index].c_str() : "None"))) {
				for (unsigned int i = 0; i < layers.size(); ++i) {
					const bool is_selected = (selected_layer_index == i);
					if (ImGui::Selectable(layer_names[i].c_str(), is_selected)) {
						selected_layer_index = i;
						bit_position = 0;
						// Copy current layer's mask ID to edit
						edit_mask_id = static_cast<unsigned int>(layers[selected_layer_index]->getLayerMask().to_ulong());
					}
					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		else {
			ImGui::Text("No layers available.");
		}

		// Show selected layer for editing
		if (selected_layer_index != -1 && selected_layer_index < layers.size()) {
			ImGui::Text("Edit Selected Layer");

			// Button to create a new layer with the next available index
			if (ImGui::Button("Create Layer")) {
				NIKE_SCENES_SERVICE->getCurrScene()->createLayer(layer_count);
				selected_layer_index = layer_count;
			}

			ImGui::SameLine();

			// Button to remove the selected layer
			if (ImGui::Button("Remove Layer")) {

				//Only able to remove layer
				if (layer_count > 1)
				{
					unsigned int layer_id = layers[selected_layer_index]->getLayerID();
					NIKE_SCENES_SERVICE->getCurrScene()->removeLayer(layer_id);

					// Update the selected index and refetch the layers
					selected_layer_index = 0;
					bit_position = 0;
				}
				else {
					ImGui::OpenPopup("Unable to remove layer");
				}
			}

			ImGui::Separator();

			// Show selected layer for editing
			if (selected_layer_index != -1 && selected_layer_index < layers.size()) {
				ImGui::Text("Edit Selected Layer");

				//Set bit position
				if (selected_layer_index == bit_position) {
					for (unsigned int i = 0; i < layers.size(); ++i) {
						// Skip the currently selected layer
						if (i == selected_layer_index)
							continue;

						bit_position = i;
						break;
					}
				}

				// Layer selection dropdown
				if (layers.size() > 1) {
					if (ImGui::BeginCombo("Select Mask Layer", layer_names.size() > 0 ? layer_names[bit_position].c_str() : "None")) {
						for (unsigned int i = 0; i < layers.size(); ++i) {
							// Skip the currently selected layer
							if (i == static_cast<unsigned int>(selected_layer_index))
								continue;

							// Check if the current layer in the loop is the selected mask layer
							const bool mask_selected = (bit_position == i);

							// Display the selectable item
							if (ImGui::Selectable(layer_names[i].c_str(), mask_selected)) {
								bit_position = i;  // Update bit_position when a new item is selected
							}

							// Set focus to the selected item if it matches
							if (mask_selected) ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					//Set bit state
					bit_state = layers[selected_layer_index]->getLayerMask().test(bit_position);
					ImGui::Checkbox("Set Bit State", &bit_state);
					layers[selected_layer_index]->setLayerMask(bit_position, bit_state);
				}
				else {
					ImGui::Text("No layers available.");
				}

				// Popup for invalid bit position error
				if (ImGui::BeginPopup("Unable to create layer")) {
					ImGui::Text("Error: Max number of 64 layers created");
					if (ImGui::Button("Close")) {
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
				if (ImGui::BeginPopup("Unable to remove layer")) {
					ImGui::Text("Unable to remove layer");
					if (ImGui::Button("Close")) {
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
			else {
				ImGui::Text("Select a layer to edit or remove.");
			}

			ImGui::End();
		}
	}

	void imguiShowGameViewport(bool& dispatch, unsigned int tex_id) {
		ImGui::Begin("Game Viewport");

		ImVec2 window_size = ImGui::GetContentRegionAvail();

		float aspect_ratio = 16.f / 9.f;
		

		float viewport_width = window_size.x;
		float viewport_height = window_size.x / aspect_ratio;

		if (viewport_height > window_size.y) {
			viewport_height = window_size.y;
			viewport_width = window_size.y * aspect_ratio;
		}

		ImTextureID textureID = (ImTextureID)tex_id;
		ImVec2 uv0(0.0f, 1.0f); // Bottom-left
		ImVec2 uv1(1.0f, 0.0f); // Top-right

		// Dispatch window viewport events
		static Vector2f win_pos = { ImGui::GetWindowPos().x + ImGui::GetStyle().FramePadding.x * 2, ImGui::GetWindowPos().y + ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.y * 2 };

		if (win_pos != Vector2f(ImGui::GetWindowPos().x + ImGui::GetStyle().FramePadding.x * 2, ImGui::GetWindowPos().y + ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.y * 2) ||
			dispatch) {
			win_pos = { ImGui::GetWindowPos().x + ImGui::GetStyle().FramePadding.x * 2, ImGui::GetWindowPos().y + ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.y * 2 };

			Vector2f win_size = { viewport_width, viewport_height };
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<IMGUI::ViewPortEvent>(win_pos, win_size));
			dispatch = false;
		}

		ImVec2 mouse_pos = ImGui::GetMousePos();
		ImVec2 scalar = { (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x / window_size.x), (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y / window_size.y) };
		mouse_pos.x -= win_pos.x;
		mouse_pos.y -= win_pos.y;
		mouse_pos.x -= (viewport_width / 2.0f);
		mouse_pos.y -= (viewport_height / 2.0f);
		mouse_pos.y = -mouse_pos.y;

		mouse_pos.x *= scalar.x;
		mouse_pos.y *= scalar.y;

		ImGui::Image(textureID, ImVec2(viewport_width, viewport_height), uv0, uv1);

		// Begin drag-and-drop target to create a new entity with the dropped texture
		// Begin drag-and-drop target to create a new entity with the dropped texture
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture")) {
				// Retrieve the texture ID from the payload
				const char* dropped_texture = static_cast<const char*>(payload->Data);
				std::string new_texture_id(dropped_texture);

				// Static counter to generate unique entity reference strings
				static int texture_counter = 1;
				std::string entity_ref = "texture_" + std::to_string(texture_counter++); // Generate unique ID

				// Create a new entity
				Entity::Type new_entity = NIKE_ECS_MANAGER->createEntity();

				// Register the entity with the generated reference
				NIKE_IMGUI_SERVICE->addEntityRef(entity_ref, new_entity);

				Vector2f def_size{ 100.0f, 100.0f };

				// Convert to own vector
				Vector2f own_mouse{mouse_pos.x , mouse_pos.y};

				// Add a Transform component to the new entity with the calculated mouse position and random size
				NIKE_ECS_MANAGER->addEntityComponent<Transform::Transform>(new_entity, Transform::Transform(own_mouse, def_size, 0.f));

				// Add a Texture component to the new entity with the specified texture ID and default color
				NIKE_ECS_MANAGER->addEntityComponent<Render::Texture>(new_entity, Render::Texture(new_texture_id, { 1.0f, 1.0f, 1.0f, 1.0f }));
			}
			ImGui::EndDragDropTarget();
		}


		ImGui::End();
	}



	void imguiCameraControl()
	{
		ImGui::Begin("Camera Control");

		static int selectedCameraIndex = 0; // Index of the currently selected camera ( REMEMBER TO CHANGE BACK TO 0)
		static std::vector<std::pair<std::string, Entity::Type>> cameraEntities; // Store camera names and their entities

		if (!NIKE_IMGUI_SERVICE->populateLists) {
			cameraEntities.clear();

			cameraEntities.emplace_back(std::string("Free Cam"), static_cast<Entity::Type>(-1));
			// Populate the cameraEntities list only once
			for (const auto& elem : NIKE_IMGUI_SERVICE->getEntityRef()) {
				if (NIKE_IMGUI_SERVICE->checkEntityExist(elem.first)) {
					Entity::Type entity = NIKE_IMGUI_SERVICE->getEntityByName(elem.first);

					// Check if the entity has a camera component
					if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Cam>(entity)) {
						cameraEntities.emplace_back(elem.first, entity);
					}
				}
			}
			NIKE_IMGUI_SERVICE->populateLists = true; // Mark as initialized to avoid re-populating
		}

		// !TODO update currently selected cam id during init to reflect in drop down
		// Create a combo box for camera selection
		if (!cameraEntities.empty()) {
			ImGui::Text("Select Camera:");

			// Lambda to retrieve items from cameraEntities for ImGui::Combo
			auto cameraNameGetter = [](void* data, int idx, const char** out_text) {
				const auto& names = *static_cast<std::vector<std::pair<std::string, Entity::Type>>*>(data);
				if (idx < 0 || idx >= names.size()) return false;
				*out_text = names[idx].first.c_str();
				return true;
				};

			// Use the lambda with ImGui::Combo
			if (ImGui::Combo("##CameraSelector", &selectedCameraIndex, cameraNameGetter, &cameraEntities, static_cast<int>(cameraEntities.size()))) {
				// Dispatch an event when the camera selection changes
				Entity::Type entity = cameraEntities[selectedCameraIndex].second;
				NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::ChangeCamEvent>(entity));

			}
		}
		else {
			ImGui::Text("No cameras available.");
		}
		ImGui::Spacing();

		// If camera selected is free cam
		if (selectedCameraIndex == 0) {
			// Position Controls
			ImGui::Text("Position:");

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

			ImGui::Spacing();
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

			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<NIKE::Render::UpdateCamEvent>(NIKE::Render::CamPosition::NONE, NIKE::Render::CamZoom::RESET_ZOOM));

		}


		ImGui::End();
	}
}

