/*****************************************************************//**
 * \file   sEditorPanels.cpp
 * \brief  Level Editor panel implementation
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/LevelEditor/sEditorPanels.h"
#include "Core/Engine.h"
#include "Systems/Render/sysRender.h"

namespace NIKE {
	/*****************************************************************//**
	* Panel Interface
	*********************************************************************/
	bool LevelEditor::IPanel::b_popup_showing = false;

	void LevelEditor::IPanel::init() {
	}

	void LevelEditor::IPanel::registerPopUp(std::string const& popup_id, std::function<void()> popup_func) {
		//Check if popup has already been registered
		if (popups.find(popup_id) != popups.end()) {
			throw std::runtime_error("Popup already registered");
		}

		//Emplace popup
		popups.emplace(popup_id, PopUp{ false, popup_func });
	}

	void LevelEditor::IPanel::editPopUp(std::string const& popup_id, std::function<void()> popup_func) {
		//Check if popup has been registered
		if (popups.find(popup_id) == popups.end()) {
			throw std::runtime_error("Popup not yet registered");
		}

		popups.at(popup_id) = PopUp{ false, popup_func };
	}

	void LevelEditor::IPanel::openPopUp(std::string const& popup_id) {
		auto it = popups.find(popup_id);

		if (it == popups.end()) {
			throw std::runtime_error("Popup doest not exist");
		}

		//Calculate the center of the viewport
		ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
		ImVec2 viewport_pos = ImGui::GetMainViewport()->Pos;
		ImVec2 popup_pos = ImVec2(viewport_pos.x + viewport_size.x * 0.5f, viewport_pos.y + viewport_size.y * 0.5f);

		//Center the popup
		ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		//Set pop management variables
		b_popup_showing = true;
		popups.at(popup_id).b_is_open = true;
		ImGui::OpenPopup(popup_id.c_str());
	}

	void LevelEditor::IPanel::closePopUp(std::string const& popup_id) {
		auto it = popups.find(popup_id);

		if (it == popups.end()) {
			throw std::runtime_error("Popup doest not exist");
		}

		b_popup_showing = false;
		popups.at(popup_id).b_is_open = false;
		ImGui::CloseCurrentPopup();
	}

	void LevelEditor::IPanel::renderPopUps() {

		//Iterate through all popup and render
		for (auto& popup : popups) {
			if (popup.second.b_is_open && ImGui::BeginPopupModal(popup.first.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
				popup.second.popUpFunction();
				ImGui::EndPopup();
			}
		}
	}

	bool LevelEditor::IPanel::checkPopUpShowing() {
		return b_popup_showing;
	}

	std::function<void()> LevelEditor::IPanel::defPopUp(std::string const& id, std::shared_ptr<std::string> msg) {
		return [this, id, msg]() {
			//Show error message
			ImGui::Text("%s", msg->c_str());

			//Add Spacing
			ImGui::Spacing();

			//OK button to close the popup
			if (ImGui::Button("OK")) {
				closePopUp(id);
			}
		};
	}

	ImVec2 LevelEditor::IPanel::worldToScreen(ImVec2 const& pos, ImVec2 const& render_size) {
		//Get window position ( Relative to top left corner of the rendering point in window )
		Vector2f window_pos = { ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y };

		//Get scale relative to the world size
		Vector2f scale{ render_size.x / NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize().x, render_size.y / NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize().y };

		//Return screen coordinates
		return {	window_pos.x + (render_size.x / 2.0f) + ((-NIKE_CAMERA_SERVICE->getActiveCamera().position.x + pos.x) * scale.x / NIKE_CAMERA_SERVICE->getActiveCamera().zoom),
					window_pos.y + (render_size.y / 2.0f) + ((NIKE_CAMERA_SERVICE->getActiveCamera().position.y + pos.y) * scale.y / NIKE_CAMERA_SERVICE->getActiveCamera().zoom) };
	}

	void LevelEditor::IPanel::worldRectFilled(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color, float rounding) {

		//Draw filled rect
		draw_list->AddRectFilled(worldToScreen(ImVec2(e_transform.position.x - (e_transform.scale.x / 2.0f), -e_transform.position.y - (e_transform.scale.y / 2.0f)), render_size),
			worldToScreen(ImVec2(e_transform.position.x + (e_transform.scale.x / 2.0f), -e_transform.position.y + (e_transform.scale.y / 2.0f)), render_size),
			color, rounding);
	}

	void LevelEditor::IPanel::worldRect(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color, float rounding, float thickness) {
		//Draw filled rect
		draw_list->AddRect(worldToScreen(ImVec2(e_transform.position.x - (e_transform.scale.x / 2.0f), -e_transform.position.y - (e_transform.scale.y / 2.0f)), render_size),
			worldToScreen(ImVec2(e_transform.position.x + (e_transform.scale.x / 2.0f), -e_transform.position.y + (e_transform.scale.y / 2.0f)), render_size),
			color, rounding, 0, thickness);
	}

	void LevelEditor::IPanel::worldQuadFilled(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color) {
		//Convert transform to vertices
		auto corners = Utility::convertTransformToVert(e_transform);

		//Draw quad bounding box
		draw_list->AddQuadFilled(worldToScreen(ImVec2(corners[0].x, corners[0].y), render_size),
			worldToScreen(ImVec2(corners[1].x, corners[1].y), render_size),
			worldToScreen(ImVec2(corners[2].x, corners[2].y), render_size),
			worldToScreen(ImVec2(corners[3].x, corners[3].y), render_size), color);
	}

	void LevelEditor::IPanel::worldQuad(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color, float thickness) {
		//Convert transform to vertices
		auto corners = Utility::convertTransformToVert(e_transform);

		//Full Screen scale
		auto fullscreen_scale = NIKE_WINDOWS_SERVICE->getWindow()->getFullScreenScale();

		//Get zoom
		auto zoom = NIKE_CAMERA_SERVICE->getActiveCamera().zoom;

		//Draw quad bounding box
		draw_list->AddQuad(worldToScreen(ImVec2(corners[0].x, corners[0].y), render_size),
			worldToScreen(ImVec2(corners[1].x, corners[1].y), render_size),
			worldToScreen(ImVec2(corners[2].x, corners[2].y), render_size),
			worldToScreen(ImVec2(corners[3].x, corners[3].y), render_size), color, thickness / zoom * fullscreen_scale.x);
	}

	void LevelEditor::IPanel::worldCircleFilled(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color) {
		//World scale factor
		Vector2f scale{ render_size.x / NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize().x, render_size.y / NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize().y };
		
		//Get zoom
		auto zoom = NIKE_CAMERA_SERVICE->getActiveCamera().zoom;

		//Render filled circle
		draw_list->AddCircleFilled(worldToScreen(ImVec2(e_transform.position.x, -e_transform.position.y), render_size),
			(e_transform.scale.x * scale.x) / 2.0f / zoom, color);
	}

	void LevelEditor::IPanel::worldCircle(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color, float thickness) {
		//World scale factor
		Vector2f scale{ render_size.x / NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize().x, render_size.y / NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize().y };

		//Full Screen scale
		auto fullscreen_scale = NIKE_WINDOWS_SERVICE->getWindow()->getFullScreenScale();

		//Get zoom
		auto zoom = NIKE_CAMERA_SERVICE->getActiveCamera().zoom;

		//Render filled circle
		draw_list->AddCircle(worldToScreen(ImVec2(e_transform.position.x, -e_transform.position.y), render_size),
			(e_transform.scale.x * scale.x) / 2.0f / zoom, color, 0, thickness / zoom * fullscreen_scale.x);
	}

	void LevelEditor::IPanel::worldTriangleFilled(ImDrawList* draw_list, Transform::Transform const& e_transform, ImGuiDir dir, ImVec2 const& render_size, ImU32 color) {
		
		//Render filled triangle based on direction
		switch (dir) {
		case ImGuiDir::ImGuiDir_Up: {
			draw_list->AddTriangleFilled(worldToScreen(ImVec2(e_transform.position.x, -e_transform.position.y - (e_transform.scale.y / 2.0f)), render_size),
				worldToScreen(ImVec2(e_transform.position.x + (e_transform.scale.x / 2.0f), -e_transform.position.y + (e_transform.scale.y / 2.0f)), render_size),
				worldToScreen(ImVec2(e_transform.position.x - (e_transform.scale.x / 2.0f), -e_transform.position.y + (e_transform.scale.y / 2.0f)), render_size),
				color);
			break;
			}
		case ImGuiDir::ImGuiDir_Down: {
			draw_list->AddTriangleFilled(worldToScreen(ImVec2(e_transform.position.x, -e_transform.position.y + (e_transform.scale.y / 2.0f)), render_size),
				worldToScreen(ImVec2(e_transform.position.x + (e_transform.scale.x / 2.0f), -e_transform.position.y - (e_transform.scale.y / 2.0f)), render_size),
				worldToScreen(ImVec2(e_transform.position.x - (e_transform.scale.x / 2.0f), -e_transform.position.y - (e_transform.scale.y / 2.0f)), render_size),
				color);
			break;
		}
		case ImGuiDir::ImGuiDir_Right: {
			draw_list->AddTriangleFilled(worldToScreen(ImVec2(e_transform.position.x + (e_transform.scale.x / 2.0f), -e_transform.position.y), render_size),
				worldToScreen(ImVec2(e_transform.position.x - (e_transform.scale.x / 2.0f), -e_transform.position.y + (e_transform.scale.y / 2.0f)), render_size),
				worldToScreen(ImVec2(e_transform.position.x - (e_transform.scale.x / 2.0f), -e_transform.position.y - (e_transform.scale.y / 2.0f)), render_size),
				color);
			break;
		}
		case ImGuiDir::ImGuiDir_Left: {
			draw_list->AddTriangleFilled(worldToScreen(ImVec2(e_transform.position.x - (e_transform.scale.x / 2.0f), -e_transform.position.y), render_size),
				worldToScreen(ImVec2(e_transform.position.x + (e_transform.scale.x / 2.0f), -e_transform.position.y + (e_transform.scale.y / 2.0f)), render_size),
				worldToScreen(ImVec2(e_transform.position.x + (e_transform.scale.x / 2.0f), -e_transform.position.y - (e_transform.scale.y / 2.0f)), render_size),
				color);
			break;
		}
		default: {
			draw_list->AddTriangleFilled(worldToScreen(ImVec2(e_transform.position.x, -e_transform.position.y - (e_transform.scale.y / 2.0f)), render_size),
				worldToScreen(ImVec2(e_transform.position.x + (e_transform.scale.x / 2.0f), -e_transform.position.y + (e_transform.scale.y / 2.0f)), render_size),
				worldToScreen(ImVec2(e_transform.position.x - (e_transform.scale.x / 2.0f), -e_transform.position.y + (e_transform.scale.y / 2.0f)), render_size),
				color);
			break;
		}
		}
	}

	void LevelEditor::IPanel::worldLine(ImDrawList* draw_list, Vector2f const& point1, Vector2f const& point2, ImVec2 const& render_size, ImU32 color, float thickness) {

		//Full Screen scale
		auto fullscreen_scale = NIKE_WINDOWS_SERVICE->getWindow()->getFullScreenScale();

		//Get zoom
		auto zoom = NIKE_CAMERA_SERVICE->getActiveCamera().zoom;

		//Draw line of rotation
		draw_list->AddLine(worldToScreen(ImVec2(point1.x, -point1.y), render_size), worldToScreen(ImVec2(point2.x, -point2.y), render_size), color, thickness / zoom * fullscreen_scale.x);
	}

	/*****************************************************************//**
	* Main Panel
	*********************************************************************/
	bool LevelEditor::MainPanel::getDebugState() const {
		return b_debug_mode;
	}

	void LevelEditor::MainPanel::setGameState(bool state) {
		b_game_state = state;

		//Get all ecs systems
		auto& systems = NIKE_ECS_MANAGER->getAllSystems();

		//Set the state of each systems based on new game state
		if (!b_game_state) {
			std::for_each(systems.begin(), systems.end(),
				[](std::shared_ptr<System::ISystem>& system) {
					if (system->getSysName() != NIKE_ECS_MANAGER->getSystemName<Render::Manager>()) {
						system->setActiveState(false);
					}
				});
		}
		else {
			std::for_each(systems.begin(), systems.end(),
				[](std::shared_ptr<System::ISystem>& system) {
					system->setActiveState(true);
				});
		}
	}

	bool LevelEditor::MainPanel::getGameState() const {
		return b_game_state;
	}

	bool LevelEditor::MainPanel::getGridState() const {
		return b_grid_state;
	}

	bool LevelEditor::MainPanel::getGizmoState() const {
		return b_gizmo_state;
	}

	void LevelEditor::MainPanel::init() {

		//Setup window flags
		window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	void LevelEditor::MainPanel::update() {
	}

	void LevelEditor::MainPanel::render() {
		//Get ImGui IO
		ImGuiIO& io = ImGui::GetIO();

		//Set up main panel position for docking
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));

		//Begin Frame
		ImGui::Begin(getName().c_str(), nullptr, static_cast<ImGuiWindowFlags>(window_flags));

		//Start Main Panel Menu Bar
		if (ImGui::BeginMenuBar()) {

			ImGui::Spacing();

			//Game State Switching
			{
				ImGui::Text("Play/Pause Game: ");
				ImGui::Button(b_game_state ? "Pause" : "Play");

				//Check if button has been activated
				if (ImGui::IsItemActivated()) {
					Action set_game_state;

					//Do game mode
					set_game_state.do_action = [&, mode = !b_game_state]() {
						setGameState(mode);
						};

					//Undo game mode
					set_game_state.undo_action = [&, mode = b_game_state]() {
						setGameState(mode);
						};

					//Execute action
					NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_game_state));
				}
			}

			ImGui::Spacing();

			//Debug Mode Switching
			{
				ImGui::Text("Show Debug: ");
				ImGui::Button(b_debug_mode ? "Hide##DebugMode" : "Show##DebugMode");

				//Check if button has been activated
				if (ImGui::IsItemActivated()) {
					Action set_debug_state;

					//Do debug mode
					set_debug_state.do_action = [&, mode = !b_debug_mode]() {
						b_debug_mode = mode;
						};

					//Undo debug mode
					set_debug_state.undo_action = [&, mode = b_debug_mode]() {
						b_debug_mode = mode;
						};

					//Execute action
					NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_debug_state));
				}
			}

			ImGui::Spacing();

			//Grid Mode Switching
			{
				ImGui::Text("Show Grid: ");
				ImGui::Button(b_grid_state ? "Hide##GridState" : "Show##GridState");

				//Check if button has been activated
				if (ImGui::IsItemActivated()) {
					Action set_grid_mode;

					//Do grid mode
					set_grid_mode.do_action = [&, mode = !b_grid_state]() {
						b_grid_state = mode;
						};

					//Undo grid mode
					set_grid_mode.undo_action = [&, mode = b_grid_state]() {
						b_grid_state = mode;
						};

					//Execute action
					NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_grid_mode));
				}
			}

			ImGui::Spacing();

			//Gizmo usage showing
			{
				ImGui::Text("Transfomation Gizmo: ");
				ImGui::Button(b_gizmo_state ? "Disable##Gizmo" : "Enable##Gizmo");

				//Check if button has been activated
				if (ImGui::IsItemActivated()) {
					Action set_gizmo_mode;

					//Do gizmo mode
					set_gizmo_mode.do_action = [&, mode = !b_gizmo_state]() {
						b_gizmo_state = mode;
						};

					//Undo gizmo mode
					set_gizmo_mode.undo_action = [&, mode = b_gizmo_state]() {
						b_gizmo_state = mode;
						};

					//Execute action
					NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_gizmo_mode));
				}
			}

			ImGui::Spacing();

			//Reset Scene
			{
				ImGui::Text("Reset Scene:");
				if (ImGui::Button("Reset")) {

					//Get scn file path tagged to active scene
					std::string curr_scn_file = NIKE_SCENES_SERVICE->getCurrScene()->getScnPath();

					//Resetting Curr Scn File
					if (!curr_scn_file.empty() && std::filesystem::exists(curr_scn_file)) {

						// Clear previous scene entities before loading the new one
						NIKE_ECS_MANAGER->destroyAllEntities();

						// Load the scene from the selected file path
						NIKE_SERIALIZE_SERVICE->loadSceneFromFile(curr_scn_file);
					}
					else {
						NIKEE_CORE_ERROR("Error: Scene file path is invalid or file does not exist.");
					}
				}
			}

			//End of menu bar
			ImGui::EndMenuBar();
		}

		//Docking
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

		//End Frame
		ImGui::End();
	}

	/*****************************************************************//**
	* Entities Panel
	*********************************************************************/
	std::function<void()> LevelEditor::EntitiesPanel::createEntityPopUp(std::string const& popup_id) {
		return [this, popup_id]() {

			//Static entity name input buffer
			static std::string entity_name;

			//Get entity text
			ImGui::Text("Enter a name for the new entity:");
			if(ImGui::InputText("##Entity Name", entity_name.data(), entity_name.capacity() + 1)){
				entity_name.resize(strlen(entity_name.c_str()));
			}

			//Static layer id
			static int layer_id = 0;

			//Get entity layer id
			ImGui::Text("Enter layer id for the new entity:");
			ImGui::InputInt("##EntityLayerIDInput", &layer_id, 1);

			//Clamp layer ID
			layer_id = std::clamp(layer_id, 0, std::clamp(static_cast<int>(NIKE_SCENES_SERVICE->getCurrScene()->getLayerCount() - 1), 0, 64));

			//If enter or ok button is pressed
			if (ImGui::Button("OK") || ImGui::GetIO().KeysDown[NIKE_KEY_ENTER]) {
				//Temporary create action
				Action create;

				//Create a shared id for do & undo functions
				std::shared_ptr<std::string> shared_id = std::make_shared<std::string>(entity_name);

				//If entity name is not provided (Create a default)
				if (shared_id->empty() || name_to_entity.find(shared_id->data()) != name_to_entity.end())
				{
					snprintf(shared_id->data(), shared_id->capacity() + 1, "entity_%04d", NIKE_ECS_MANAGER->getEntitiesCount());
				}

				//Do Action
				create.do_action = [&, shared_id]() {
					//Creat new entity 
					Entity::Type new_id = NIKE_ECS_MANAGER->createEntity(layer_id);

					//Save entity name into entities ref
					entities.emplace(new_id, EditorEntity());
					entity_to_name.emplace(new_id, shared_id->c_str());
					name_to_entity.emplace(shared_id->c_str(), new_id);
				};

				//Undo Action
				create.undo_action = [&, shared_id]() {

					//Check if entity is still alive
					if (name_to_entity.find(shared_id->data()) != name_to_entity.end()) {
						//Destroy new entity
						NIKE_ECS_MANAGER->destroyEntity(name_to_entity.at(shared_id->data()));

						//Erase new entity ref
						entities.erase(name_to_entity.at(shared_id->data()));
						entity_to_name.erase(name_to_entity.at(shared_id->data()));
						name_to_entity.erase(shared_id->data());
					}
				};

				//Execute create entity action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(create));

				//Reset layer id
				layer_id = 0;

				//Reset entity name
				entity_name.clear();

				//Close popup
				closePopUp(popup_id);
			}

			ImGui::SameLine();

			//Cancel creating new entity
			if (ImGui::Button("Cancel")) {

				//Reset layer id
				layer_id = 0;

				//Reset entity name
				entity_name.clear();

				//Close popup
				closePopUp(popup_id);
			}
		};
	}

	std::function<void()> LevelEditor::EntitiesPanel::removeEntityPopUp(std::string const& popup_id) {
		return [this, popup_id]() {

			//Confirm removal of entity
			ImGui::Text("Are you sure you want to remove %s?", entity_to_name.at(selected_entity).c_str());

			//If enter or ok button is pressed
			if (ImGui::Button("Remove") || ImGui::GetIO().KeysDown[NIKE_KEY_ENTER]) {

				//Temporary remove action
				Action remove;

				//Create a shared id for do & undo functions
				std::shared_ptr<std::string> shared_id = std::make_shared<std::string>(entity_to_name.at(selected_entity));

				//Get all entity comps for pass by value storage
				auto comps = NIKE_ECS_MANAGER->getAllCopiedEntityComponents(selected_entity);
				auto comp_types = NIKE_ECS_MANAGER->getAllComponentTypes();
				int layer_id = NIKE_ECS_MANAGER->getEntityLayerID(selected_entity);

				//Setup undo action for remove
				remove.undo_action = [&, shared_id, comps, comp_types, layer_id]() {

					//Creat new entity 
					Entity::Type new_id = NIKE_ECS_MANAGER->createEntity(layer_id);

					//Add all the comps back
					for (auto&& comp : comps) {
						NIKE_ECS_MANAGER->addDefEntityComponent(new_id, comp_types.at(comp.first));
						NIKE_ECS_MANAGER->setEntityComponent(new_id, comp_types.at(comp.first), comp.second);
					}

					//Update entities list
					entities.emplace(new_id, EditorEntity());

					//Save entity name into entities ref
					entity_to_name.emplace(new_id, shared_id->c_str());
					name_to_entity.emplace(shared_id->c_str(), new_id);

					//Set selected entity back to old entity
					selected_entity = name_to_entity.at(shared_id->data());
				};

				//Setup action for removing entity
				remove.do_action = [&, shared_id]() {

					//Check if entity is still alive
					if (name_to_entity.find(shared_id->data()) != name_to_entity.end()) {
						//Destroy entity
						NIKE_ECS_MANAGER->destroyEntity(name_to_entity.at(shared_id->data()));

						//Update entities list
						entities.erase(name_to_entity.at(shared_id->data()));

						//Erase new entity ref
						entity_to_name.erase(name_to_entity.at(shared_id->data()));
						name_to_entity.erase(shared_id->data());

						//Set selected entity back to first entity
						selected_entity = entities.empty() ? 0 : entities.begin()->first;
					}
				};

				//Execute remove action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(remove));

				//Close popup
				closePopUp(popup_id);
			}

			ImGui::SameLine();

			//Cancel removing entity
			if (ImGui::Button("Cancel")) {

				//Close popup
				closePopUp(popup_id);
			}
		};
	}

	std::function<void()> LevelEditor::EntitiesPanel::cloneEntityPopUp(std::string const& popup_id) {
		return [this, popup_id]() {

			//Static entity name input buffer
			static std::string entity_name;

			//Get entity text
			ImGui::Text("Enter a new name for the cloned entity:");
			if (ImGui::InputText("##Entity Clone Name", entity_name.data(), entity_name.capacity() + 1)) {
				entity_name.resize(strlen(entity_name.c_str()));
			}

			//If enter or clone button is pressed
			if (ImGui::Button("Clone") || ImGui::GetIO().KeysDown[NIKE_KEY_ENTER]) {
				//Temporary clone action
				Action clone;

				//Create a shared id for do & undo functions
				std::shared_ptr<std::string> shared_id = std::make_shared<std::string>(entity_name);

				//If entity name is not provided (Create a default)
				if (shared_id->empty() || name_to_entity.find(shared_id->data()) != name_to_entity.end())
				{
					snprintf(shared_id->data(), shared_id->capacity() + 1, "entity_%04d", NIKE_ECS_MANAGER->getEntitiesCount());
				}

				//Clone entity for capturing by value
				Entity::Type clone_entity = selected_entity;

				//Do Action
				clone.do_action = [&, shared_id, clone_entity]() {
					if (NIKE_ECS_MANAGER->checkEntity(clone_entity)) {
						//Clone entity 
						Entity::Type new_id = NIKE_ECS_MANAGER->cloneEntity(clone_entity);

						//Save entity name into entities ref
						entities.emplace(new_id, EditorEntity());
						entity_to_name.emplace(new_id, shared_id->c_str());
						name_to_entity.emplace(shared_id->c_str(), new_id);
					}
				};

				//Undo Action
				clone.undo_action = [&, shared_id]() {

					//Check if entity is still alive
					if (name_to_entity.find(shared_id->data()) != name_to_entity.end()) {
						//Destroy new entity
						NIKE_ECS_MANAGER->destroyEntity(name_to_entity.at(shared_id->data()));

						//Erase new entity ref
						entities.erase(name_to_entity.at(shared_id->data()));
						entity_to_name.erase(name_to_entity.at(shared_id->data()));
						name_to_entity.erase(shared_id->data());
					}
				};

				//Execute create entity action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(clone));

				//Reset entity name
				entity_name.clear();

				//Close popup
				closePopUp(popup_id);
			}

			ImGui::SameLine();

			//Cancel creating new entity
			if (ImGui::Button("Cancel")) {

				//Reset entity name
				entity_name.clear();

				//Close popup
				closePopUp(popup_id);
			}
		};
	}

	void LevelEditor::EntitiesPanel::init() {

		//Register popups
		registerPopUp("Create Entity", createEntityPopUp("Create Entity"));
		registerPopUp("Remove Entity", removeEntityPopUp("Remove Entity"));
		registerPopUp("Clone Entity", cloneEntityPopUp("Clone Entity"));
		error_msg = std::make_shared<std::string>("Error");
		registerPopUp("Error", defPopUp("Error", error_msg));

		//Game panel reference
		game_panel = std::dynamic_pointer_cast<GameWindowPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(GameWindowPanel::getStaticName()));

		//Tile map panel reference
		tilemap_panel = std::dynamic_pointer_cast<TileMapPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(TileMapPanel::getStaticName()));

		//Components panel reference
		comp_panel = std::dynamic_pointer_cast<ComponentsPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(ComponentsPanel::getStaticName()));
	}

	void LevelEditor::EntitiesPanel::update() {

		//Get all active entities
		if (entities.size() != NIKE_ECS_MANAGER->getEntitiesCount()) {
			auto entities_set = NIKE_ECS_MANAGER->getAllEntities();
			//Iterate through all active entities
			unsigned int count = 0;
			for (auto entity : entities_set) {
				if (entities.find(entity) == entities.end()) {
					//Emplace entity
					entities.emplace(entity, EditorEntity());

					//Create identifier for entity
					char entity_name[32];
					snprintf(entity_name, sizeof(entity_name), "entity_%04d", count);

					//Add entity to BiMap
					entity_to_name.emplace(entity, entity_name);
					name_to_entity.emplace(entity_name, entity);
				}

				count++;
			}

			if (entities.size() != entities_set.size()) {
				//Iterate through entities ref to check which entity has been removed
				auto enit = entity_to_name.begin();
				auto neit = name_to_entity.begin();
				for (decltype(entities)::iterator it = entities.begin(); it != entities.end();) {
					if (entities_set.find(it->first) == entities_set.end()) {
						it = entities.erase(it);
						enit = entity_to_name.erase(enit);
						neit = name_to_entity.erase(neit);
					}
					else {
						++it;
						++enit;
						++neit;
					}
				}
			}
		}
	}

	void LevelEditor::EntitiesPanel::render() {
		ImGui::Begin(getName().c_str());

		// Button to create an entity, which triggers the popup
		if (ImGui::Button("Create") && entities.size() < Entity::MAX) {
			openPopUp("Create Entity");
		}

		//Buttons Same Line
		ImGui::SameLine();

		// Button to remove an entity, which triggers the popup
		if ((ImGui::Button("Remove") || ImGui::GetIO().KeysDown[ImGuiKey_Delete]) && (entities.find(selected_entity) != entities.end()) && !entities.empty()) {
			openPopUp("Remove Entity");
		}

		//Buttons Same Line
		ImGui::SameLine();

		// Button to clone an entity, which triggers the popup
		if (ImGui::Button("Clone") && (entities.find(selected_entity) != entities.end()) && entities.size() < Entity::MAX) {
			openPopUp("Clone Entity");
		}

		//Add Spacing
		ImGui::Spacing();

		//Show number of entities in the level
		ImGui::Text("Number of entities in level: %d", entities.size());

		//Add Spacing
		ImGui::Spacing();

		//Reset entity changed
		b_entity_changed = false;

		//Check if there are entities present
		if (!entities.empty()) {

			//Check for disable entity interaction flag
			if (!checkPopUpShowing() && game_panel.lock()->isMouseInWindow() && !comp_panel.lock()->checkGizmoInteraction() && !tilemap_panel.lock()->checkGridEditing()) {
				//Reverse Iterate through layers to check for entity being clicked
				static bool entity_clicked = false;
				entity_clicked = false;

				//Iterate through entities from top layer down
				for (auto layer = NIKE_SCENES_SERVICE->getCurrScene()->getLayers().rbegin();
					!entity_clicked && layer != NIKE_SCENES_SERVICE->getCurrScene()->getLayers().rend();
					layer++) {

					//SKip inactive layer
					if (!layer->get()->getLayerState())
						continue;

					//Iterate through all entities
					for (auto& entity : entities) {

						//Skip entities not on curr layer
						if (layer->get()->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity.first))
							continue;

						//Check for entity clicking
						if (isCursorInEntity(entity.first) && ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left]) {
							selected_entity = entity.first;

							//Get selected entity data
							auto it = entity_to_name.find(selected_entity);

							//Signal entity changed
							b_entity_changed = true;

							//Signal that an entity has been clicked
							entity_clicked = true;
							break;
						}
					}
				}
			}

			//Iterate through all entities to showcase active entities
			for (auto& entity : entities) {

				//Check if entity is selected
				bool selected = (entities.find(selected_entity) != entities.end()) && entity_to_name.at(entity.first).c_str() == entity_to_name.at(selected_entity).c_str();

				//Show selectable
				if (ImGui::Selectable(entity_to_name.at(entity.first).c_str(), selected)) {

					//Select entity
					if (selected_entity != entity.first) {
						selected_entity = entity.first;

						//Get selected entity data
						auto it = entity_to_name.find(selected_entity);
						b_entity_changed = true;
					}
					else {
						unselectEntity();
						b_entity_changed = true;
					}

					//Check here if tilemap is editing grid
					if (tilemap_panel.lock()->checkGridEditing()) {
						error_msg->assign("Editing grid now, unable to select entity.");
						openPopUp("Error");
						unselectEntity();
					}
				}
			}
		}

		//Render popups
		renderPopUps();

		ImGui::End();
	}

	std::string LevelEditor::EntitiesPanel::getEntityName(Entity::Type entity) {
		auto it = entity_to_name.find(entity);

		if (it == entity_to_name.end()) {
			throw std::runtime_error("Entity not found");
		}

		return it->second;
	}

	Entity::Type LevelEditor::EntitiesPanel::getSelectedEntity() const {
		return selected_entity;
	}

	std::optional<std::string> LevelEditor::EntitiesPanel::getSelectedEntityName() const {
		//Get selected entity data
		auto it = entity_to_name.find(selected_entity);

		if (it != entity_to_name.end()) {
			return it->second;
		}
		else {
			return "";
		}
	}

	void LevelEditor::EntitiesPanel::unselectEntity() {
		selected_entity = UINT16_MAX;
	}

	std::optional<std::reference_wrapper<LevelEditor::EditorEntity>> LevelEditor::EntitiesPanel::getSelectedEntityEditor() {
		//Get selected entity data
		auto it = entities.find(selected_entity);

		if (it != entities.end()) {
			return it->second;
		}
		else {
			return std::nullopt;
		}
	}

	void LevelEditor::EntitiesPanel::lockAllEntities() {
		for (auto& entity : entities) {
			entity.second.b_locked = true;
		}
	}

	void LevelEditor::EntitiesPanel::unlockAllEntities() {
		for (auto& entity : entities) {
			entity.second.b_locked = false;
		}
	}

	bool LevelEditor::EntitiesPanel::isEntityChanged() const {
		return b_entity_changed;
	}

	bool LevelEditor::EntitiesPanel::isCursorInEntity(Entity::Type entity) const {
		auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
		if (e_transform_comp.has_value()) {

			//Get transform
			const auto& e_transform = e_transform_comp.value().get();

			// Retrieve the cursor position in world space
			Vector2f cursorWorldPos = game_panel.lock()->getWorldMousePos();

			//Return result
			return Utility::isCursorInTransform(cursorWorldPos, e_transform);
		}
		else {
			return false;
		}
	}

	/*****************************************************************//**
	* Components Panel
	*********************************************************************/
	void LevelEditor::ComponentsPanel::interactGizmo() {
		//Check if entity is locked
		if (!entities_panel.lock()->getSelectedEntityEditor().has_value() || entities_panel.lock()->getSelectedEntityEditor().value().get().b_locked) {
			return;
		}

		//Get transform component
		auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entities_panel.lock()->getSelectedEntity());
		if (!e_transform_comp.has_value())
			return;

		//Get transform
		auto& e_transform = e_transform_comp.value().get();

		//Get mouse pos
		Vector2f world_mouse = game_panel.lock()->getWorldMousePos();

		//Render for each gizmo mode
		switch (gizmo.mode) {
		case GizmoMode::Translate: {
			//Extra object for translate ( Move box )
			gizmo.objects["Move Box"].first.position = { e_transform.position.x + (e_transform.scale.x * 0.75f), e_transform.position.y + (e_transform.scale.y * 0.75f) };
			gizmo.objects["Move Box"].first.scale = { e_transform.scale.x * 0.25f, e_transform.scale.y * 0.25f };
			gizmo.objects["Move Box"].second = { 100, 100, 100, 255 };

			//Interaction with move box
			if (game_panel.lock()->isMouseInWindow() && !checkPopUpShowing() && ((gizmo.b_dragging_hori && gizmo.b_dragging_vert) || ((!gizmo.b_dragging_hori && !gizmo.b_dragging_vert) && Utility::isCursorInTransform(world_mouse, gizmo.objects["Move Box"].first))) && ImGui::GetIO().MouseDown[ImGuiMouseButton_Left]) {
				//Set dragging flags
				gizmo.b_dragging_hori = true;
				gizmo.b_dragging_vert = true;

				//Initialize on mouse click
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					//Update prev transform
					gizmo.prev_transform = e_transform;
				}

				//Apply transformation
				e_transform.position = { world_mouse.x - (e_transform.scale.x * 0.75f),  -world_mouse.y - (e_transform.scale.y * 0.75f) };
			}

			//Add gizmo up
			gizmo.objects["Up"].first.position = { e_transform.position.x, e_transform.position.y + (e_transform.scale.y / 2.0f) };
			gizmo.objects["Up"].first.scale = { e_transform.scale.x * 0.05f, e_transform.scale.y };
			gizmo.objects["Up"].second = { 0, 255, 0, 255 };

			//Add gizmo up point
			gizmo.objects["Up Point"].first.position = { e_transform.position.x, e_transform.position.y + e_transform.scale.y };
			gizmo.objects["Up Point"].first.scale = { e_transform.scale.x * 0.25f, e_transform.scale.y * 0.25f };
			gizmo.objects["Up Point"].second = { 0, 255, 0, 255 };

			//Interaction with up
			if (game_panel.lock()->isMouseInWindow() && !checkPopUpShowing() && ((!gizmo.b_dragging_hori && gizmo.b_dragging_vert) || Utility::isCursorInTransform(world_mouse, gizmo.objects["Up"].first) || Utility::isCursorInTransform(world_mouse, gizmo.objects["Up Point"].first)) && ImGui::GetIO().MouseDown[ImGuiMouseButton_Left]) {
				//Set dragging flags
				gizmo.b_dragging_vert = true;
				gizmo.b_dragging_hori = false;

				//Static variables to store initial values
				static float initial_mouse_y = 0.0f;
				static float initial_position_y = 0.0f;

				//Initialize on mouse click
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					initial_mouse_y = world_mouse.y;
					initial_position_y = e_transform.position.y;

					//Update prev transform
					gizmo.prev_transform = e_transform;
				}

				//Calculate the Y-axis delta
				float delta_y = -(world_mouse.y - initial_mouse_y) * gizmo.sensitivity;

				//Apply the delta to the position
				e_transform.position.y = initial_position_y + delta_y;
			}

			//Add gizmo right
			gizmo.objects["Right"].first.position = { e_transform.position.x + (e_transform.scale.x / 2.0f), e_transform.position.y };
			gizmo.objects["Right"].first.scale = { e_transform.scale.x, e_transform.scale.y * 0.05f };
			gizmo.objects["Right"].second = { 255, 0, 0, 255 };

			//Add gizmo right point
			gizmo.objects["Right Point"].first.position = { e_transform.position.x + e_transform.scale.x, e_transform.position.y };
			gizmo.objects["Right Point"].first.scale = { e_transform.scale.x * 0.25f, e_transform.scale.y * 0.25f };
			gizmo.objects["Right Point"].second = { 255, 0, 0, 255 };

			//Interaction with right
			if (game_panel.lock()->isMouseInWindow() && !checkPopUpShowing() && ((gizmo.b_dragging_hori && !gizmo.b_dragging_vert) || Utility::isCursorInTransform(world_mouse, gizmo.objects["Right"].first) || Utility::isCursorInTransform(world_mouse, gizmo.objects["Right Point"].first)) && ImGui::GetIO().MouseDown[ImGuiMouseButton_Left]) {
				
				//Set dragging flags
				gizmo.b_dragging_vert = false;
				gizmo.b_dragging_hori = true;
				
				//Static variables to store initial values
				static float initial_mouse_x = 0.0f;
				static float initial_position_x = 0.0f;

				//Initialize on mouse click
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					initial_mouse_x = world_mouse.x;
					initial_position_x = e_transform.position.x;

					//Update prev transform
					gizmo.prev_transform = e_transform;
				}

				//Calculate the X-axis delta
				float delta_x = (world_mouse.x - initial_mouse_x) * gizmo.sensitivity;

				//Apply the delta to the position
				e_transform.position.x = initial_position_x + delta_x;
			}

			//Add gizmo center
			gizmo.objects["Center"].first.position = { e_transform.position.x, e_transform.position.y };
			gizmo.objects["Center"].first.scale = { e_transform.scale.x * 0.25f, e_transform.scale.y * 0.25f };
			gizmo.objects["Center"].second = { 255, 255, 255, 255 };

			//Dragging stopped
			if ((gizmo.b_dragging_hori || gizmo.b_dragging_vert) && ImGui::GetIO().MouseReleased[ImGuiMouseButton_Left]) {
				
				//Reset dragging flags
				gizmo.b_dragging_hori = false;
				gizmo.b_dragging_vert = false;

				//Check if grid snapping is enabled
				if (checkGridSnapping()) {

					//Get snapped to cell position
					auto cursor_cell = NIKE_MAP_SERVICE->getCellAtPosition(e_transform.position);
					if (cursor_cell.has_value()) {

						//Snap to cell
						e_transform.position = cursor_cell.value().get().position;
					}
				}

				//Apply action
				LevelEditor::Action change_pos;

				//Change pos do action
				change_pos.do_action = [&, pos = e_transform.position]() {
					e_transform.position = pos;
					};

				//Change pos undo action
				change_pos.undo_action = [&, pos = gizmo.prev_transform.position]() {
					e_transform.position = pos;
					};

				//Execute action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_pos));
			}

			break;
		}
		case GizmoMode::Scale: {
			//Add gizmo up
			gizmo.objects["Up"].first.position = { e_transform.position.x, e_transform.position.y + (e_transform.scale.y / 2.0f) };
			gizmo.objects["Up"].first.scale = { e_transform.scale.x * 0.05f, e_transform.scale.y };
			gizmo.objects["Up"].second = { 0, 255, 0, 255 };

			//Add gizmo up point
			gizmo.objects["Up Point"].first.position = { e_transform.position.x, e_transform.position.y + e_transform.scale.y };
			gizmo.objects["Up Point"].first.scale = { e_transform.scale.x * 0.25f, e_transform.scale.y * 0.25f };
			gizmo.objects["Up Point"].second = { 0, 255, 0, 255 };

			//Interaction with up
			if (game_panel.lock()->isMouseInWindow() && !checkPopUpShowing() && ((!gizmo.b_dragging_hori && gizmo.b_dragging_vert) || Utility::isCursorInTransform(world_mouse, gizmo.objects["Up"].first) || Utility::isCursorInTransform(world_mouse, gizmo.objects["Up Point"].first)) && ImGui::GetIO().MouseDown[ImGuiMouseButton_Left]) {

				//Set dragging flags
				gizmo.b_dragging_vert = true;
				gizmo.b_dragging_hori = false;

				//Static variables to store initial values
				static float initial_mouse_y = 0.0f;
				static float initial_scale_y = 0.0f;

				//Initialize on mouse click
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					initial_mouse_y = world_mouse.y;
					initial_scale_y = e_transform.scale.y;
					
					//Update prev transform
					gizmo.prev_transform = e_transform;
				}

				//Calculate scale sensitivity
				float scale_sensitivity = gizmo.sensitivity / (initial_scale_y + EPSILON);

				//Calculate the Y-axis movement relative to the initial position
				float delta_y = -(world_mouse.y - initial_mouse_y);
				
				//Calculate scale factor
				float scale_factor = (1.0f + delta_y * scale_sensitivity) < EPSILON ? EPSILON : (1.0f + delta_y * scale_sensitivity);

				//Adjust the scale proportionally
				e_transform.scale.y = initial_scale_y * scale_factor;

				//Clamp the scale
				e_transform.scale.y = std::clamp(e_transform.scale.y, EPSILON, (float)UINT16_MAX);
			}

			//Add gizmo right
			gizmo.objects["Right"].first.position = { e_transform.position.x + (e_transform.scale.x / 2.0f), e_transform.position.y };
			gizmo.objects["Right"].first.scale = { e_transform.scale.x, e_transform.scale.y * 0.05f };
			gizmo.objects["Right"].second = { 255, 0, 0, 255 };

			//Add gizmo right point
			gizmo.objects["Right Point"].first.position = { e_transform.position.x + e_transform.scale.x, e_transform.position.y };
			gizmo.objects["Right Point"].first.scale = { e_transform.scale.x * 0.25f, e_transform.scale.y * 0.25f };
			gizmo.objects["Right Point"].second = { 255, 0, 0, 255 };

			//Interaction with right
			if (game_panel.lock()->isMouseInWindow() && !checkPopUpShowing() && ((gizmo.b_dragging_hori && !gizmo.b_dragging_vert) || Utility::isCursorInTransform(world_mouse, gizmo.objects["Right"].first) || Utility::isCursorInTransform(world_mouse, gizmo.objects["Right Point"].first)) && ImGui::GetIO().MouseDown[ImGuiMouseButton_Left]) {

				//Set dragging flags
				gizmo.b_dragging_vert = false;
				gizmo.b_dragging_hori = true;

				//Static variables to store initial values
				static float initial_mouse_x = 0.0f;
				static float initial_scale_x = 0.0f;

				//Initialize on mouse click
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					initial_mouse_x = world_mouse.x;
					initial_scale_x = e_transform.scale.x;

					//Update prev transform
					gizmo.prev_transform = e_transform;
				}

				//Calculate scale sensitivity
				float scale_sensitivity = gizmo.sensitivity / (initial_scale_x + EPSILON);

				//Calculate the X-axis movement relative to the initial position
				float delta_x = (world_mouse.x - initial_mouse_x);

				//Calculate scale factor
				float scale_factor = (1.0f + delta_x * scale_sensitivity) < EPSILON ? EPSILON : (1.0f + delta_x * scale_sensitivity);

				//Adjust the scale proportionally
				e_transform.scale.x = initial_scale_x * scale_factor;

				//Clamp the scale
				e_transform.scale.x = std::clamp(e_transform.scale.x, EPSILON, (float)UINT16_MAX);
			}

			//Add gizmo center
			gizmo.objects["Center"].first.position = { e_transform.position.x, e_transform.position.y };
			gizmo.objects["Center"].first.scale = { e_transform.scale.x * 0.25f, e_transform.scale.y * 0.25f };
			gizmo.objects["Center"].second = { 255, 255, 255, 255 };

			//Dragging stopped
			if ((gizmo.b_dragging_hori || gizmo.b_dragging_vert) && ImGui::GetIO().MouseReleased[ImGuiMouseButton_Left]) {
				//Reset dragging flags
				gizmo.b_dragging_hori = false;
				gizmo.b_dragging_vert = false;

				//Apply action
				LevelEditor::Action change_scale;

				//Change scale do action
				change_scale.do_action = [&, scale = e_transform.scale]() {
					e_transform.scale = scale;
					};

				//Change scale undo action
				change_scale.undo_action = [&, scale = gizmo.prev_transform.scale]() {
					e_transform.scale = scale;
					};

				//Execute action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_scale));
			}

			break;
		}
		case GizmoMode::Rotate: {

			//Add rotation circle
			gizmo.objects["Rot Circle"].first.position = { e_transform.position.x, e_transform.position.y };
			gizmo.objects["Rot Circle"].first.scale = { Utility::getMax(e_transform.scale.x * 1.5f, e_transform.scale.y * 1.5f), Utility::getMax(e_transform.scale.x * 1.5f, e_transform.scale.y * 1.5f) };
			gizmo.objects["Rot Circle"].second = { 255, 255, 255, 255 };

			//Click on circle and change rotation point
			if (game_panel.lock()->isMouseInWindow() && !checkPopUpShowing() && Utility::isCursorInTransform(world_mouse, gizmo.objects["Rot Circle"].first) && ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left]) {
				//Prev angle for undo and redo
				float prev_angle = e_transform.rotation;

				//Calculate mouse angle relative to the circle's center
				float angle = atan2(-(world_mouse.y + gizmo.objects["Rot Circle"].first.position.y), world_mouse.x - gizmo.objects["Rot Circle"].first.position.x);

				//Wrap angle
				if (angle < 0.0f) angle += 2.0f * static_cast<float>(M_PI);

				//Convert angle from radians to degrees
				float angle_deg = angle * (180.0f / static_cast<float>(M_PI));

				//Update the entity's rotation angle
				e_transform.rotation = std::clamp(angle_deg, 0.0f, 360.0f);

				//Apply action
				LevelEditor::Action change_rotation;

				//Change rotation do action
				change_rotation.do_action = [&, rotation = e_transform.rotation]() {
					e_transform.rotation = rotation;
					};

				//Change rotation undo action
				change_rotation.undo_action = [&, rotation = prev_angle]() {
					e_transform.rotation = rotation;
					};

				//Execute action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_rotation));
			}

			//Add rotation point
			gizmo.objects["Rot Point"].first.position = {
				gizmo.objects["Rot Circle"].first.position.x + (gizmo.objects["Rot Circle"].first.scale.x * 0.5f) * cos(e_transform.rotation * static_cast<float>(M_PI) / 180.0f),
				gizmo.objects["Rot Circle"].first.position.y + (gizmo.objects["Rot Circle"].first.scale.x * 0.5f) * sin(e_transform.rotation * static_cast<float>(M_PI) / 180.0f)
			};
			gizmo.objects["Rot Point"].first.scale = { e_transform.scale.x * 0.25f, e_transform.scale.y * 0.25f };
			gizmo.objects["Rot Point"].second = { 100, 100, 100, 255 };

			//Check for interactiom with rotation point
			if (game_panel.lock()->isMouseInWindow() && !checkPopUpShowing() && ((gizmo.b_dragging_hori && gizmo.b_dragging_vert) || Utility::isCursorInTransform(world_mouse, gizmo.objects["Rot Point"].first)) && ImGui::GetIO().MouseDown[ImGuiMouseButton_Left]) {

				//Set dragging flags
				gizmo.b_dragging_vert = true;
				gizmo.b_dragging_hori = true;

				//Initialize on mouse click
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					//Update prev transform
					gizmo.prev_transform = e_transform;
				}

				//Calculate mouse angle relative to the circle's center
				float angle = atan2(-(world_mouse.y + gizmo.objects["Rot Circle"].first.position.y), world_mouse.x - gizmo.objects["Rot Circle"].first.position.x);

				//Wrap angle
				if (angle < 0.0f) angle += 2.0f * static_cast<float>(M_PI);

				//Convert angle from radians to degrees
				float angle_deg = angle * (180.0f / static_cast<float>(M_PI));

				//Update the entity's rotation angle
				e_transform.rotation = std::clamp(angle_deg, 0.0f, 360.0f);
			}

			//Dragging stopped
			if ((gizmo.b_dragging_hori && gizmo.b_dragging_vert) && ImGui::GetIO().MouseReleased[ImGuiMouseButton_Left]) {
				//Reset dragging flags
				gizmo.b_dragging_hori = false;
				gizmo.b_dragging_vert = false;

				//Apply action
				LevelEditor::Action change_rotation;

				//Change rotation do action
				change_rotation.do_action = [&, rotation = e_transform.rotation]() {
					e_transform.rotation = rotation;
					};

				//Change rotation undo action
				change_rotation.undo_action = [&, rotation = gizmo.prev_transform.rotation]() {
					e_transform.rotation = rotation;
					};

				//Execute action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_rotation));
			}

			break;
		}
		default: {
			break;
		}
		}

		//Iterate through all gizmo objects and add hover interaction
		for (auto& object : gizmo.objects) {

			//Apply small scale when cursor is in gizmo object
			if (Utility::isCursorInTransform(world_mouse, object.second.first)) {

				//Set flag for interaction with gizmo
				gizmo.b_interacting = true;

				//Skip the rotation circle
				if (object.first == "Rot Circle") {
					continue;
				}

				//Objects hover factor
				object.second.first.scale.x *= 1.1f;
				object.second.first.scale.y *= 1.1f;
				object.second.second.r = static_cast<int>(object.second.second.r * 0.85f);
				object.second.second.g = static_cast<int>(object.second.second.g * 0.85f);
				object.second.second.b = static_cast<int>(object.second.second.b * 0.85f);
			}
		}
	}

	std::function<void()> LevelEditor::ComponentsPanel::addComponentPopUp(std::string const& popup_id) {
		return [this, popup_id]() {

			//Select a component to add
			ImGui::Text("Select a component to add:");

			//Add spacing before components
			ImGui::Spacing();

			//Iterate over all registered components
			for (const auto& component : comps) {

				//Check if component already exists
				if (NIKE_ECS_MANAGER->checkEntityComponent(entities_panel.lock()->getSelectedEntity(), component.second))
					continue;

				//Display each component as a button
				if (ImGui::Button(component.first.c_str())) {

					//Temporary add component action
					Action add_comp;

					//Setup undo action for add component
					add_comp.undo_action = [=]() {
						if (NIKE_ECS_MANAGER->checkEntityComponent(entities_panel.lock()->getSelectedEntity(), component.second)) {
							NIKE_ECS_MANAGER->removeEntityComponent(entities_panel.lock()->getSelectedEntity(), component.second);
						}
					};

					//Setup do action for add component
					add_comp.do_action = [=]() {

						//Add default comp to entity
						NIKE_ECS_MANAGER->addDefEntityComponent(entities_panel.lock()->getSelectedEntity(), component.second);
					};

					//Execute add component action
					NIKE_LVLEDITOR_SERVICE->executeAction(std::move(add_comp));

					//Close popup
					closePopUp(popup_id);
				}
			}

			//Add Spacing after components
			ImGui::Spacing();

			//Cancel removing entity
			if (ImGui::Button("Cancel")) {

				//Close popup
				closePopUp(popup_id);
			}
		};
	}

	std::function<void()> LevelEditor::ComponentsPanel::savePrefabPopUp(std::string const& popup_id) {
		return[this, popup_id]() {
		
			static std::string prefab_file_path;

			ImGui::Text("Enter file path to save prefab:");
			if (ImGui::InputText("##PrefabFilePath", prefab_file_path.data(), prefab_file_path.capacity() + 1)) {
				prefab_file_path.resize(strlen(prefab_file_path.c_str()));
			}

			if (ImGui::Button("OK") || ImGui::GetIO().KeysDown[NIKE_KEY_ENTER]) {
				//Temporary create action
				Action save_prefab;

				// For saving of the prefab file with the extension
				std::string prefab_full_path = NIKE_ASSETS_SERVICE->getPrefabsPath() + prefab_file_path + ".prefab";

				//Create a shared id for do & undo functions
				std::shared_ptr<std::string> shared_id = std::make_shared<std::string>(prefab_full_path);

				//Do Action
				save_prefab.do_action = [&, shared_id]() {
					// Serialize the prefab to the file path
					NIKE_SERIALIZE_SERVICE->saveEntityToFile(entities_panel.lock()->getSelectedEntity(), *shared_id);
					setPopUpSuccessMsg("Prefab file saved!");
					openPopUp("Success");

					};

				//Undo Action
				save_prefab.undo_action = [&, shared_id]() {

					const std::string& file_to_delete = *shared_id;

					// Check if file exists before deleting
					if (std::filesystem::exists(file_to_delete)) {
						try {
							std::filesystem::remove(file_to_delete);
						}
						catch (const std::filesystem::filesystem_error& e) {
							// Log error if file deletion fails
							NIKEE_CORE_WARN("Failed to delete prefab file during undo: " + std::string(e.what()));
						}
					}
					else {
						NIKEE_CORE_WARN("File does not exist for undo: " + file_to_delete);
					}
					};

				//Execute create entity action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_prefab));

				//Reset entity name
				prefab_file_path.clear();

				//Close popup
				closePopUp(popup_id);
			}

			ImGui::SameLine();

			//Cancel saving prefab
			if (ImGui::Button("Cancel")) {

				//Reset entity name
				prefab_file_path.clear();

				//Close popup
				closePopUp(popup_id);
			}
		};
	}

	std::function<void()> LevelEditor::ComponentsPanel::setLayerIDPopUp(std::string const& popup_id) {
		return [this, popup_id]() {

			//Set a layer ID
			ImGui::Text("Set Layer ID For Entity");

			//Add spacing
			ImGui::Spacing();

			//Static layer id
			static int layer_id = 0;
			if (ImGui::IsItemActivated()) {
				layer_id = NIKE_ECS_MANAGER->getEntityLayerID(entities_panel.lock()->getSelectedEntity());
			}

			//Input int
			ImGui::InputInt("##NewLayerID", &layer_id, 1);

			//Clamp layer ID
			layer_id = std::clamp(layer_id, 0, std::clamp(static_cast<int>(NIKE_SCENES_SERVICE->getCurrScene()->getLayerCount() - 1), 0, 64));

			//Click set to set layer
			if (ImGui::Button("Set")) {

				//Temporary set layer action
				Action set_layer;

				//Setup undo action for set layer
				set_layer.undo_action = [&, id = NIKE_ECS_MANAGER->getEntityLayerID(entities_panel.lock()->getSelectedEntity())]() {
					NIKE_ECS_MANAGER->setEntityLayerID(entities_panel.lock()->getSelectedEntity(), id);
					};

				//Setup do action for set layer
				set_layer.do_action = [&, id = layer_id]() {
					NIKE_ECS_MANAGER->setEntityLayerID(entities_panel.lock()->getSelectedEntity(), id);
					};

				//Execute set layer action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_layer));

				//Close popup
				closePopUp(popup_id);
			}

			//Add Spacing
			ImGui::Spacing();

			//Cancel setting layer id
			if (ImGui::Button("Cancel")) {

				//Close popup
				closePopUp(popup_id);
			}
		};
	}

	void LevelEditor::ComponentsPanel::init() {

		//Entities panel reference
		entities_panel = std::dynamic_pointer_cast<EntitiesPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(EntitiesPanel::getStaticName()));

		//Game panel reference
		game_panel = std::dynamic_pointer_cast<GameWindowPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(GameWindowPanel::getStaticName()));

		//Main panel reference
		main_panel = std::dynamic_pointer_cast<MainPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(MainPanel::getStaticName()));

		//Tilemap panel reference
		tilemap_panel = std::dynamic_pointer_cast<TileMapPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(TileMapPanel::getStaticName()));

		//Register add component popup
		registerPopUp("Add Component", addComponentPopUp("Add Component"));
		registerPopUp("Set Layer ID", setLayerIDPopUp("Set Layer ID"));
		registerPopUp("Save Prefab", savePrefabPopUp("Save Prefab"));
		error_msg = std::make_shared<std::string>("Comp Error");
		success_msg = std::make_shared<std::string>("Saving Success");
		registerPopUp("Error", defPopUp("Error", error_msg));
		registerPopUp("Success", defPopUp("Success", success_msg));
	}

	void LevelEditor::ComponentsPanel::update() {

		//Update components list
		if (comps.size() != NIKE_ECS_MANAGER->getComponentsCount()) {

			//Get updated components list
			comps = NIKE_ECS_MANAGER->getAllComponentTypes();

			//Init comp ui funcs with empty funcs
			std::for_each(comps.begin(), comps.end(), [&](std::pair<std::string, Component::Type> const& comp) {
				if (comps_ui.find(comp.first) == comps_ui.end()) {
					comps_ui.emplace(comp.first, [](ComponentsPanel&, void*) {});
				}
			});
		}
	}

	void LevelEditor::ComponentsPanel::render() {
		ImGui::Begin(getName().c_str());

		//Reset gizmo interaction
		gizmo.b_interacting = false;

		//Check if an entity has been selected
		if (NIKE_ECS_MANAGER->checkEntity(entities_panel.lock()->getSelectedEntity())) {

			//Print out selected entity string ref
			ImGui::Text("Selected Entity: %s", entities_panel.lock()->getSelectedEntityName().value_or("").c_str());

			//Print out selected entity component count
			ImGui::Text("Number of Components in entity: %d", NIKE_ECS_MANAGER->getEntityComponentCount(entities_panel.lock()->getSelectedEntity()));

			//Print out selected entity layer id
			ImGui::Text("Entity's Layer: %d", NIKE_ECS_MANAGER->getEntityLayerID(entities_panel.lock()->getSelectedEntity()));

			//Entity locking
			{
				//Lock entity
				ImGui::Text("Lock Entity: ");
				ImGui::SameLine();
				if (entities_panel.lock()->getSelectedEntityEditor().has_value()) {
					auto& locked = entities_panel.lock()->getSelectedEntityEditor().value().get().b_locked;
					if (ImGui::SmallButton(locked ? "Locked" : "Unlocked")) {
						locked = !locked;
					}

					//Return if entity is locked
					if (locked) {
						ImGui::End();
						return;
					}
				}
				else {
					//Return if entity editor does not have value
					return;
				}
			}

			//Transformation gizmo is enabled
			if (main_panel.lock()->getGizmoState()) {
				//Add Spacing
				ImGui::Spacing();

				//Add Separator
				ImGui::Separator();

				//Add Spacing
				ImGui::Spacing();

				//Gizmo functionalities
				{
					//Toggle gizmo state
					{
						ImGui::Text("Gizmo Mode: ");

						//Array of gizmo mode names
						const char* gizmo_mode_names[] = { "Translate", "Scale", "Rotate" };
						int current_mode = static_cast<int>(gizmo.mode);
						static int prev_mode = static_cast<int>(gizmo.mode);

						//Render the dropdown
						if (ImGui::Combo("##Gizmo Mode", &current_mode, gizmo_mode_names, IM_ARRAYSIZE(gizmo_mode_names))) {

							//Set gizmo mode
							Action set_gizmo_mode;

							//Do gizmo mode
							set_gizmo_mode.do_action = [&, mode = current_mode]() {
								gizmo.mode = static_cast<GizmoMode>(mode);
								};

							//Undo gizmo mode
							set_gizmo_mode.undo_action = [&, mode = prev_mode]() {
								gizmo.mode = static_cast<GizmoMode>(mode);
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_gizmo_mode));

							//Set prev mode to current mode
							prev_mode = current_mode;

							//Clear gizmo objects
							gizmo.objects.clear();
						}
					}

					//Set Gizmo Sensitivity
					{
						ImGui::Text("Set Gizmo Sensitivity: ");
						ImGui::DragFloat("##GizmoSensitivity", &gizmo.sensitivity, 0.01f, 0.0f, 5.0f);

						//Before editing
						static float sensitivity_before_change = gizmo.sensitivity;
						if (ImGui::IsItemActivated()) {
							sensitivity_before_change = gizmo.sensitivity;
						}

						//Upon editing finish
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							//Set gizmo sensi
							Action set_gizmo_sensi;

							//Do gizmo sensi
							set_gizmo_sensi.do_action = [&, sensi = gizmo.sensitivity]() {
								gizmo.sensitivity = sensi;
								};

							//Undo gizmo sensi
							set_gizmo_sensi.undo_action = [&, sensi = sensitivity_before_change]() {
								gizmo.sensitivity = sensi;
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_gizmo_sensi));
						}
					}

					//Interact with gizmo
					if (main_panel.lock()->getGizmoState()) {
						interactGizmo();
					}
				}
			}

			//Add Spacing
			ImGui::Spacing();

			//Add Separator
			ImGui::Separator();

			//Add Spacing
			ImGui::Spacing();

			//Add component popup
			if (ImGui::Button("Add Component")) {
				openPopUp("Add Component");
			}

			ImGui::SameLine();

			//Set layer id popup
			if (ImGui::Button("Set Layer ID")) {
				openPopUp("Set Layer ID");
			}

			ImGui::SameLine();

			//Save prefab popup
			if (ImGui::Button("Save Prefab")) {
				openPopUp("Save Prefab");
			}

			//Add Spacing
			ImGui::Spacing();

			//Retrieve and display all registered component types
			for (auto& comp : NIKE_ECS_MANAGER->getAllEntityComponents(entities_panel.lock()->getSelectedEntity())) {

				//Create a collapsible header for the component
				if (ImGui::CollapsingHeader(comp.first.c_str(), ImGuiTreeNodeFlags_None)) {

					//Display Component UI
					comps_ui.at(comp.first)(*this, comp.second.get());

					//Add Spacing
					ImGui::Spacing();

					//Remove component button
					if (ImGui::Button(std::string("Remove Component##" + comp.first).c_str())) {
						Action remove_comp;

						//Values to copy
						Component::Type comp_type_copy = comps.at(comp.first);
						Entity::Type entity_copy = entities_panel.lock()->getSelectedEntity();
						auto comp_copy = NIKE_ECS_MANAGER->getCopiedEntityComponent(entities_panel.lock()->getSelectedEntity(), comp_type_copy);

						//Do Action
						remove_comp.do_action = [&, entity_copy, comp_type_copy]() {
							NIKE_ECS_MANAGER->removeEntityComponent(entity_copy, comp_type_copy);
						};

						//Undo Action
						remove_comp.undo_action = [&, entity_copy, comp_type_copy, comp_copy]() {
							NIKE_ECS_MANAGER->addDefEntityComponent(entity_copy, comp_type_copy);
							NIKE_ECS_MANAGER->setEntityComponent(entity_copy, comp_type_copy, comp_copy);
						};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(remove_comp));
					}
				}
			}
		}
		else {
			ImGui::Text("Select an entity.");
		}

		//Render popups
		renderPopUps();

		ImGui::End();
	}

	bool LevelEditor::ComponentsPanel::isEntityChanged() const {
		return entities_panel.lock()->isEntityChanged();
	}

	void LevelEditor::ComponentsPanel::setPopUpErrorMsg(std::string const& msg) {
		error_msg->assign(msg);
	}

	void LevelEditor::ComponentsPanel::setPopUpSuccessMsg(std::string const& msg)
	{
		success_msg->assign(msg);
	}

	void LevelEditor::ComponentsPanel::renderEntityBoundingBox(void* draw_list, Vector2f const& render_size) {
		//Check if entity is locked
		if (!entities_panel.lock()->getSelectedEntityEditor().has_value() || entities_panel.lock()->getSelectedEntityEditor().value().get().b_locked) {
			return;
		}

		//Get transform component
		auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entities_panel.lock()->getSelectedEntity());
		if (!e_transform_comp.has_value())
			return;

		auto const& e_transform = e_transform_comp.value().get();

		//Internal imgui draw
		auto draw = static_cast<ImDrawList*>(draw_list);

		//Convert rendersize
		ImVec2 rendersize = { render_size.x, render_size.y };

		if (gizmo.mode == GizmoMode::Rotate && main_panel.lock()->getGizmoState()) {
			//Render rotation circle
			auto const& rotation_circle = gizmo.objects["Rot Circle"];
			worldCircle(draw, rotation_circle.first, rendersize, IM_COL32(rotation_circle.second.r, rotation_circle.second.g, rotation_circle.second.b, rotation_circle.second.a), (e_transform.scale.length() * 0.02f));
		}
		else {
			//Render rotated rectangle
			worldQuad(draw, e_transform, rendersize, IM_COL32(255, 255, 255, 255), (e_transform.scale.length() * 0.02f));
		}
	}

	void LevelEditor::ComponentsPanel::renderEntityGizmo(void* draw_list, Vector2f const& render_size) {
		//Check if entity is locked
		if (!entities_panel.lock()->getSelectedEntityEditor().has_value() || entities_panel.lock()->getSelectedEntityEditor().value().get().b_locked) {
			return;
		}

		//Get transform component
		auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entities_panel.lock()->getSelectedEntity());
		if (!e_transform_comp.has_value())
			return;

		//Gizmo text buffer
		std::string gizmo_text;
		gizmo_text.resize(64);

		//Get transform
		auto& e_transform = e_transform_comp.value().get();

		//Internal imgui draw
		auto draw = static_cast<ImDrawList*>(draw_list);

		//Convert rendersize
		ImVec2 rendersize = { render_size.x, render_size.y };

		//Render for each gizmo mode
		switch (gizmo.mode) {
		case GizmoMode::Translate: {

			//Edit gizmo text buffer
			std::snprintf(gizmo_text.data(), gizmo_text.capacity() + 1, "X: %.2f Y: %.2f", e_transform.position.x, e_transform.position.y);

			//Draw move box
			auto const& move_box = gizmo.objects["Move Box"];
			worldRectFilled(draw, move_box.first, rendersize, IM_COL32(move_box.second.r, move_box.second.g, move_box.second.b, move_box.second.a));

			//Draw up arrow
			auto const& up = gizmo.objects["Up"];
			worldRectFilled(draw, up.first, rendersize, IM_COL32(up.second.r, up.second.g, up.second.b, up.second.a));

			//Draw up arrow point
			auto const& up_point = gizmo.objects["Up Point"];
			worldTriangleFilled(draw, up_point.first, ImGuiDir::ImGuiDir_Up, rendersize, IM_COL32(up_point.second.r, up_point.second.g, up_point.second.b, up_point.second.a));

			//Draw right arrow
			auto const& right = gizmo.objects["Right"];
			worldRectFilled(draw, right.first, rendersize, IM_COL32(right.second.r, right.second.g, right.second.b, right.second.a));

			//Draw right arrow point
			auto const& right_point = gizmo.objects["Right Point"];
			worldTriangleFilled(draw, right_point.first, ImGuiDir::ImGuiDir_Right, rendersize, IM_COL32(right_point.second.r, right_point.second.g, right_point.second.b, right_point.second.a));
			
			//Draw center
			auto const& center = gizmo.objects["Center"];
			worldCircleFilled(draw, center.first, rendersize, IM_COL32(center.second.r, center.second.g, center.second.b, center.second.a));

			break;
			}
		case GizmoMode::Scale: {
			//Edit gizmo text buffer
			std::snprintf(gizmo_text.data(), gizmo_text.capacity() + 1, "X: %.2f Y: %.2f", e_transform.scale.x, e_transform.scale.y);

			//Draw up arrow
			auto const& up = gizmo.objects["Up"];
			worldRectFilled(draw, up.first, rendersize, IM_COL32(up.second.r, up.second.g, up.second.b, up.second.a));

			//Draw up arrow point
			auto const& up_point = gizmo.objects["Up Point"];
			worldCircleFilled(draw, up_point.first, rendersize, IM_COL32(up_point.second.r, up_point.second.g, up_point.second.b, up_point.second.a));

			//Draw right arrow
			auto const& right = gizmo.objects["Right"];
			worldRectFilled(draw, right.first, rendersize, IM_COL32(right.second.r, right.second.g, right.second.b, right.second.a));

			//Draw up arrow point
			auto const& right_point = gizmo.objects["Right Point"];
			worldCircleFilled(draw, right_point.first, rendersize, IM_COL32(right_point.second.r, right_point.second.g, right_point.second.b, right_point.second.a));

			//Draw center
			auto const& center = gizmo.objects["Center"];
			worldCircleFilled(draw, center.first, rendersize, IM_COL32(center.second.r, center.second.g, center.second.b, center.second.a));

			break;
			}
		case GizmoMode::Rotate: {
			//Edit gizmo text buffer
			std::snprintf(gizmo_text.data(), gizmo_text.capacity() + 1, "%.2f Deg", e_transform.rotation);

			//Get rotation point transform
			auto const& rotation_point = gizmo.objects["Rot Point"];

			//Draw line of rotation
			worldLine(draw, e_transform.position, rotation_point.first.position, rendersize, IM_COL32(255, 255, 255, 255), (e_transform.scale.length() * 0.02f));

			//Draw point of rotation
			worldCircleFilled(draw, rotation_point.first, rendersize, IM_COL32(rotation_point.second.r, rotation_point.second.g, rotation_point.second.b, rotation_point.second.a));
			break;
			}
		default: {
			break;
			}
		}

		//Render gizmo text
		if (gizmo.mode == GizmoMode::Rotate) {
			auto const& rotation_circle = gizmo.objects["Rot Circle"].first;
			draw->AddText(worldToScreen(ImVec2(rotation_circle.position.x - (rotation_circle.scale.x / 2.0f), -rotation_circle.position.y + (rotation_circle.scale.y * 0.6f)), rendersize), IM_COL32(255, 255, 255, 255), gizmo_text.c_str());
		}
		else {
			draw->AddText(worldToScreen(ImVec2(e_transform.position.x - (e_transform.scale.x / 2.0f), -e_transform.position.y + (e_transform.scale.y * 0.6f)), rendersize), IM_COL32(255, 255, 255, 255), gizmo_text.c_str());
		}
	}

	bool LevelEditor::ComponentsPanel::checkEntityDragged() const {
		return b_dragging_entity;
	}

	bool LevelEditor::ComponentsPanel::checkGizmoInteraction() const {
		return gizmo.b_interacting;
	}

	bool LevelEditor::ComponentsPanel::checkGridSnapping() const {
		return tilemap_panel.lock()->checkGridSnapping();
	}

	/*****************************************************************//**
	* Debugging Tools Panel
	*********************************************************************/
	void LevelEditor::DebugPanel::init() {

	}

	void LevelEditor::DebugPanel::update() {

	}

	void LevelEditor::DebugPanel::render() {
		ImGui::Begin(getName().c_str());

		//Begin tab bar
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
				auto sys_percentages = NIKE_DEBUG_SERVICE->getSystemPercentages();
				ImGui::Separator();
				if (sys_percentages.empty()) {
					ImGui::Text("No active systems to report on.");
				}
				else {
					ImGui::Text("System Performance (Percentage of total game loop time):");
					ImGui::Spacing();
					for (const auto& [name, time] : sys_percentages) {
						double barPercent = time / NIKE_DEBUG_SERVICE->getTotalSystemTime();
						ImGui::Text("%s : %.2f ms", name.c_str(), time);
						ImGui::ProgressBar(static_cast<float>(barPercent), ImVec2(-1, 0));
					}
					ImGui::Spacing();
					ImGui::Text("Total Active System Time: %.2f ms", NIKE_DEBUG_SERVICE->getTotalSystemTime());
				}

				ImGui::EndTabItem();
			}

			// Crash Logger Tab
			if (ImGui::BeginTabItem("Crash Logger")) {
				ImGui::Text("Crash logs:");

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

		//Render popups
		renderPopUps();

		ImGui::End();
	}

	/*****************************************************************//**
	* Resource Management Panel
	*********************************************************************/
	void LevelEditor::ResourcePanel::init() {

	}

	void LevelEditor::ResourcePanel::update() {

	}

	void LevelEditor::ResourcePanel::render() {
		ImGui::Begin(getName().c_str());

		// Tabs for different asset types
		if (ImGui::BeginTabBar("Asset Types"))
		{
			// Levels tab for .prefabs files
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
			if (ImGui::BeginTabItem("Models"))
			{
				displayAssetList("Models");
				ImGui::EndTabItem();
			}

			// Font tab
			if (ImGui::BeginTabItem("Fonts"))
			{
				displayAssetList("Fonts");
				ImGui::EndTabItem();
			}

			// Scripts tab
			if (ImGui::BeginTabItem("Scripts"))
			{
				displayAssetList("Scripts");
				ImGui::EndTabItem();
			}

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

	/*****************************************************************//**
	* Camera Management Panel
	*********************************************************************/
	void LevelEditor::CameraPanel::cameraChangeAction(Render::Cam& active_cam, Render::Cam& cam_before_change) {

		if (ImGui::IsItemActivated()) {
			cam_before_change = active_cam;
		}

		//Check if any item is active
		if (ImGui::IsItemDeactivated()) {
			//Camera variables changed
			Action camera_change;

			//Camera change do action
			camera_change.do_action = [&, change = active_cam]() {
				active_cam = change;
				cam_before_change = change;
				};

			//Camera change undo action
			camera_change.undo_action = [&, change = cam_before_change]() {
				active_cam = change;
				cam_before_change = change;
				};

			//Execute action
			NIKE_LVLEDITOR_SERVICE->executeAction(std::move(camera_change));
		}
	}

	void LevelEditor::CameraPanel::init() {
		entities_panel = std::dynamic_pointer_cast<EntitiesPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(EntitiesPanel::getStaticName()));

		//Setup free cam to be referenced as default camera in camera system
		free_cam = std::make_shared<Render::Cam>(Vector2f(0.0f, 0.0f), 1.0f);
	}

	void LevelEditor::CameraPanel::update() {

		//Update list of camera entities
		if (cam_entities.size() != NIKE_ECS_MANAGER->getComponentEntitiesCount(NIKE_ECS_MANAGER->getComponentType<Render::Cam>()) + 1) {
			cam_entities.clear();
			cam_entities.emplace(UINT16_MAX, "Free Cam");
			for (auto entity : NIKE_ECS_MANAGER->getAllComponentEntities(NIKE_ECS_MANAGER->getComponentType<Render::Cam>())) {
				cam_entities.emplace(entity, entities_panel.lock()->getEntityName(entity));
			}
		}
	}

	void LevelEditor::CameraPanel::render() {
		ImGui::Begin(getName().c_str());

		//Select camera
		ImGui::Text("Select Camera:");

		//Static for tracking last dispatched index & dispatching of new camera
		static bool dispatch = true;
		static int last_dispatched_index = 0;

		//Lamda for retrieving camera name
		auto cam_name = [](void* data, int idx, const char** out_text) -> bool {
			const auto& names = *static_cast<std::unordered_map<Entity::Type, std::string>*> (data);
			if (idx < 0 || idx >= names.size()) return false;
			auto it = names.begin();
			std::advance(it, idx);

			//Retrieve the entity name and assign it to out_text
			*out_text = it->second.c_str();
			return true;
			};

		// Use the lambda with ImGui::Combo
		if(ImGui::Combo("##CameraSelector", &combo_index, cam_name, &cam_entities, static_cast<int>(cam_entities.size()))) {
			dispatch = true;
		}

		//If dispatch is actived
		if (dispatch) {
			// Dispatch an event when the camera selection changes
			auto it = cam_entities.begin();
			std::advance(it, combo_index);
			auto before_it = cam_entities.begin();
			std::advance(before_it, last_dispatched_index);

			//Change camera action
			Action change_cam_action;

			//Change cam do action
			change_cam_action.do_action = [&, cam = it->first, index = combo_index]() {
				if (cam == UINT16_MAX) {
					NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Render::ChangeCamEvent>(cam, free_cam));
				}
				else {
					NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Render::ChangeCamEvent>(cam));
				}

				combo_index = index;
				last_dispatched_index = index;
			};

			//Change cam undo action
			change_cam_action.undo_action = [&, cam = before_it->first, index = last_dispatched_index]() {
				if (cam == UINT16_MAX) {
					NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Render::ChangeCamEvent>(cam, free_cam));
				}
				else {
					NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Render::ChangeCamEvent>(cam));
				}

				combo_index = index;
				last_dispatched_index = index;
			};

			//Execute action
			NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_cam_action));

			//Update dispatch to false
			dispatch = false;
		}

		ImGui::Spacing();

		//Render cam reference
		auto it = cam_entities.begin();
		std::advance(it, combo_index);
		auto e_cam_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(it->first);
		Render::Cam& active_cam = (NIKE_ECS_MANAGER->checkEntity(it->first) && e_cam_comp.has_value())
			? e_cam_comp.value().get() : *free_cam;

		//Static camera variables for undo/redo
		static Render::Cam cam_before_change = active_cam;

		//If free camera is active
		if (it->first == UINT16_MAX) {
			// Position Controls
			ImGui::Text("Position:");

			if (ImGui::Button("Up") || ImGui::IsItemActive()) {
				// Move camera position up
				active_cam.position.y += 500.0f * ImGui::GetIO().DeltaTime;
			}

			cameraChangeAction(active_cam, cam_before_change);

			ImGui::SameLine();

			if (ImGui::Button("Down") || ImGui::IsItemActive()) {
				// Move camera position down
				active_cam.position.y -= 500.0f * ImGui::GetIO().DeltaTime;
			}

			cameraChangeAction(active_cam, cam_before_change);

			ImGui::SameLine();

			if (ImGui::Button("Left") || ImGui::IsItemActive()) {
				// Move camera position left
				active_cam.position.x -= 500.0f * ImGui::GetIO().DeltaTime;
			}

			cameraChangeAction(active_cam, cam_before_change);

			ImGui::SameLine();

			if (ImGui::Button("Right") || ImGui::IsItemActive()) {
				// Move camera position right
				active_cam.position.x += 500.0f * ImGui::GetIO().DeltaTime;
			}

			cameraChangeAction(active_cam, cam_before_change);

			ImGui::Spacing();

			if (ImGui::Button("Reset Position")) {
				// Move camera position right
				active_cam.position = { 0.0f, 0.0f };
			}

			cameraChangeAction(active_cam, cam_before_change);

			ImGui::Spacing();
		}

		// Zoom Controls
		ImGui::Text("Zoom:");
		if (ImGui::Button("Zoom In") || ImGui::IsItemActive()) {
			active_cam.zoom -= 1.0f * ImGui::GetIO().DeltaTime;
			active_cam.zoom = std::clamp(active_cam.zoom, EPSILON, (float)UINT16_MAX);
		}

		cameraChangeAction(active_cam, cam_before_change);

		ImGui::SameLine();

		if (ImGui::Button("Zoom Out") || ImGui::IsItemActive()) {
			active_cam.zoom += 1.0f * ImGui::GetIO().DeltaTime;
			active_cam.zoom = std::clamp(active_cam.zoom, EPSILON, (float)UINT16_MAX);
		}

		cameraChangeAction(active_cam, cam_before_change);

		ImGui::Spacing();

		if (ImGui::Button("Reset Zoom")) {
			active_cam.zoom = 1.0f;
		}

		cameraChangeAction(active_cam, cam_before_change);

		//Render popups
		renderPopUps();

		ImGui::End();
	}

	/*****************************************************************//**
	* Tile Map Management Panel
	*********************************************************************/
	void LevelEditor::TileMapPanel::init() {
		entities_panel = std::dynamic_pointer_cast<EntitiesPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(EntitiesPanel::getStaticName()));
		registerPopUp("Save Grid", saveGridPopUp("Save Grid"));
		error_msg = std::make_shared<std::string>("Saving Error");
		success_msg = std::make_shared<std::string>("Saving Success");
		registerPopUp("Error", defPopUp("Error", error_msg));
		registerPopUp("Success", defPopUp("Success", success_msg));
	}

	void LevelEditor::TileMapPanel::update() {

		//Clicking to set map cells to blocked
		auto game_window = std::dynamic_pointer_cast<GameWindowPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(GameWindowPanel::getStaticName()));
		if (!checkPopUpShowing() && b_grid_edit && game_window->isMouseInWindow() && NIKE_MAP_SERVICE->getCursorCell().has_value() && ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left]) {
			NIKE_MAP_SERVICE->getCursorCell().value().get().b_blocked = !NIKE_MAP_SERVICE->getCursorCell().value().get().b_blocked;
		}
	}

	void LevelEditor::TileMapPanel::render() {
		ImGui::Begin(getName().c_str());


		ImGui::Spacing();


		if (ImGui::Button("Save Grid"))
		{
			// For saving of the prefab file with the extension
			std::string curr_scene = NIKE_SERIALIZE_SERVICE->getCurrSceneFile();

			std::string grid_file_name = Utility::extractFileName(curr_scene) + ".grid";

			std::string grid_full_path = NIKE_ASSETS_SERVICE->getGridsPath() + grid_file_name;

			// Serialize the grid data using the grid service
			nlohmann::json grid_data = NIKE_MAP_SERVICE->serialize();

			// Open the file for writing
			std::ofstream file(grid_full_path, std::ios::out | std::ios::trunc);

			// Check if the file opened successfully
			if (!file.is_open()) {
				openPopUp("Error");
			}
			else
			{
				openPopUp("Success");
			}

			// Write the serialized grid data to the file
			file << grid_data.dump(4);
			file.close();
		}


		//Adjust grid mode
		{
			//Adjust grid mode
			ImGui::Text("Set grid mode: ");
			ImGui::SameLine();
			ImGui::SmallButton(b_grid_edit ? "Editing" : "View");

			//Check if button has been activated
			if (ImGui::IsItemActivated()) {
				Action set_grid_mode;

				//Do grid mode
				set_grid_mode.do_action = [&, mode = !b_grid_edit]() {
					b_grid_edit = mode;
					entities_panel.lock()->unselectEntity();
					};

				//Undo grid mode
				set_grid_mode.undo_action = [&, mode = b_grid_edit]() {
					b_grid_edit = mode;
					};

				//Execute action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_grid_mode));
			}
		}

		ImGui::Spacing();

		//Adjust snap to grid mode
		{
			//Adjust cell size
			ImGui::Text("Snap entity to grid: ");
			ImGui::SameLine();
			ImGui::SmallButton(b_snap_to_grid ? "Disable##Snapping" : "Enable##Snapping");

			//Check if button has been activated
			if (ImGui::IsItemActivated()) {
				Action set_grid_snapping;

				//Do grid snapping
				set_grid_snapping.do_action = [&, mode = !b_snap_to_grid]() {
					b_snap_to_grid = mode;
					};

				//Undo grid snapping
				set_grid_snapping.undo_action = [&, mode = b_snap_to_grid]() {
					b_snap_to_grid = mode;
					};

				//Execute action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_grid_snapping));
			}
		}

		ImGui::Spacing();

		//Show grid scale
		{
			auto grid_scale = NIKE_MAP_SERVICE->getGridScale();
			ImGui::Text("Grid scale: %.3f, %.3f", grid_scale.x, grid_scale.y);
		}

		ImGui::Spacing();

		//Adjust grid size
		{
			//Static grid size
			static Vector2i grid_size = NIKE_MAP_SERVICE->getGridSize();
			static Vector2i grid_size_before_change;

			//Adjust grid size
			ImGui::Text("Adjust grid size: ");
			ImGui::DragInt2("##Grid Size", &grid_size.x, 0.1f, 0, INT16_MAX);

			//Check if grid has begun editing
			if (ImGui::IsItemActivated()) {
				grid_size_before_change = NIKE_MAP_SERVICE->getGridSize();
			}

			//Check if grid has finished editing
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				Action change_grid_size;

				//Change grid size action
				change_grid_size.do_action = [&, grid = grid_size]() {
					NIKE_MAP_SERVICE->setGridSize(grid);
					grid_size = grid;
					};

				//Undo change grid size
				change_grid_size.undo_action = [&, grid = grid_size_before_change]() {
					NIKE_MAP_SERVICE->setGridSize(grid);
					grid_size = grid;
					};

				//Execute action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_grid_size));
			}
		}

		ImGui::Spacing();

		//Adjust cell size
		{
			//Static cell size
			static Vector2f cell_size = NIKE_MAP_SERVICE->getCellSize();
			static Vector2f cell_size_before_change;

			//Adjust cell size
			ImGui::Text("Adjust cell size: ");
			ImGui::DragFloat2("##Cell Size", &cell_size.x, 0.1f, 0, INT16_MAX);

			//Check if cell has begun editing
			if (ImGui::IsItemActivated()) {
				cell_size_before_change = NIKE_MAP_SERVICE->getCellSize();
			}

			//Check if cell has finished editing
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				Action change_cell_size;

				//Change cell size action
				change_cell_size.do_action = [&, cell = cell_size]() {
					NIKE_MAP_SERVICE->setCellSize(cell);
					cell_size = cell;
					};

				//Undo change cell size
				change_cell_size.undo_action = [&, cell = cell_size_before_change]() {
					NIKE_MAP_SERVICE->setCellSize(cell);
					cell_size = cell;
					};

				//Execute action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_cell_size));
			}
		}

		ImGui::Spacing();

		//Adjust grid color
		{
			static Vector4f color_before_change;
			ImGui::Text("Adjust grid color: ");
			ImGui::ColorPicker4("Grid Color", &grid_color.r, ImGuiColorEditFlags_AlphaBar);

			//Check if color has begun editing
			if (ImGui::IsItemActivated()) {
				color_before_change = grid_color;
			}

			//Check if thickness has finished editing
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				Action change_color;

				//Change color action
				change_color.do_action = [&, color = grid_color]() {
					grid_color = color;
					};

				//Undo change color
				change_color.undo_action = [&, color = color_before_change]() {
					grid_color = color;
					};

				//Execute action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_color));
			}
		}

		ImGui::Spacing();

		//Adjust grid thickness
		{
			static float thickness_before_change = 0.0f;
			ImGui::Text("Adjust grid thickness: ");
			ImGui::DragFloat("##Grid Thickness", &grid_thickness, 0.1f, 1.0f, 10.0f);

			//Check if thickness has begun editing
			if (ImGui::IsItemActivated()) {
				thickness_before_change = grid_thickness;
			}

			//Check if thickness has finished editing
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				Action change_thickness;

				//Change thickness action
				change_thickness.do_action = [&, thickness = grid_thickness]() {
					grid_thickness = thickness;
					};

				//Undo change thickness
				change_thickness.undo_action = [&, thickness = thickness_before_change]() {
					grid_thickness = thickness;
					};

				//Execute action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_thickness));
			}
		}

		renderPopUps();

		ImGui::End();
	}

	std::function<void()> LevelEditor::TileMapPanel::saveGridPopUp(std::string const& popup_id)
	{
		return [this, popup_id]() {
			// Static buffer for file name input
			static std::string file_name;

			// Ask user for a file name to save the grid
			ImGui::Text("Enter a name for the grid file:");
			if (ImGui::InputText("##GridFileName", file_name.data(), file_name.capacity() + 1)) {
				file_name.resize(strlen(file_name.c_str()));
			}

			// Check if the file name is valid (not empty)
			if (file_name.empty()) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "File name cannot be empty!");
			}

			// If the user clicks "OK" or presses Enter, serialize the grid
			if ((ImGui::Button("OK") || ImGui::GetIO().KeysDown[NIKE_KEY_ENTER]) && !file_name.empty()) {

				// For saving of the prefab file with the extension
				std::string grid_full_path = NIKE_ASSETS_SERVICE->getGridsPath() + file_name + ".grid";

				// Serialize the grid data using the grid service
				nlohmann::json grid_data = NIKE_MAP_SERVICE->serialize();

				// Open the file for writing
				std::ofstream file(grid_full_path, std::ios::out | std::ios::trunc);

				// Check if the file opened successfully
				if (!file.is_open()) {
					NIKEE_CORE_ERROR("FILE CANNOT BE OPEN");
				}

				// Write the serialized grid data to the file
				file << grid_data.dump(4);
				file.close();

				// Close the popup after saving
				closePopUp(popup_id);

				// Clear the file name for future saves
				file_name.clear();
			}

			ImGui::SameLine();

			// If the user clicks "Cancel", reset and close the popup
			if (ImGui::Button("Cancel")) {
				file_name.clear();  

				// Close the popup
				closePopUp(popup_id);
			}
		};
	}

	void LevelEditor::TileMapPanel::renderGrid(void* draw_list, Vector2f const& render_size) {

		//Internal imgui draw
		auto draw = static_cast<ImDrawList*>(draw_list);

		//Get Grid Size
		auto grid_size = NIKE_MAP_SERVICE->getGridSize();

		//Get cell size
		auto cell_size = NIKE_MAP_SERVICE->getCellSize();

		//Get grid scale
		auto grid_scale = NIKE_MAP_SERVICE->getGridScale();

		//Full Screen scale
		auto fullscreen_scale = NIKE_WINDOWS_SERVICE->getWindow()->getFullScreenScale();

		//Calculate limits
		float top =		-(grid_scale.y / 2.0f);
		float bot =		(grid_scale.y / 2.0f);
		float left =	-(grid_scale.x / 2.0f);
		float right	=	(grid_scale.x / 2.0f);

		//Convert rendersize
		ImVec2 rendersize = { render_size.x, render_size.y };

		//Convert color
		ImU32 color = IM_COL32(grid_color.r * 255, grid_color.g * 255, grid_color.b * 255, grid_color.a * 255);

		//Calculate grid render thickness
		float gird_render_thickness = grid_thickness / NIKE_CAMERA_SERVICE->getActiveCamera().zoom;

		//Add lines for grid for rows
		for (int i = 0; i <= grid_size.y; i++) {
			draw->AddLine(worldToScreen({ left, top + (cell_size.y * i) }, rendersize), worldToScreen({ right, top + (cell_size.y * i)}, rendersize), color, gird_render_thickness * fullscreen_scale.x);
		}

		//Add lines for grid for cols
		for (int j = 0; j <= grid_size.x; j++) {
			draw->AddLine(worldToScreen({ left + (cell_size.x * j), top }, rendersize), worldToScreen({ left + (cell_size.x * j) , bot }, rendersize), color, gird_render_thickness * fullscreen_scale.x);
		}

		//Render dark hue over blocked squaress
		for (auto const& row : NIKE_MAP_SERVICE->getGrid()) {
			for (auto const& cell : row) {
				if (cell.b_blocked) {
					draw->AddRectFilled(worldToScreen({ cell.position.x - (cell_size.x / 2.0f) + (grid_thickness / fullscreen_scale.x),  cell.position.y - (cell_size.y / 2.0f) + (grid_thickness / fullscreen_scale.x) }, rendersize),
										worldToScreen({ cell.position.x + (cell_size.x / 2.0f) - (grid_thickness / fullscreen_scale.x),  cell.position.y + (cell_size.y / 2.0f) - (grid_thickness / fullscreen_scale.x) }, rendersize), IM_COL32(0, 0, 0, 100));
				}
			}
		}
	}

	bool LevelEditor::TileMapPanel::checkGridEditing() const {
		return b_grid_edit;
	}

	bool LevelEditor::TileMapPanel::checkGridSnapping() const {
		return b_snap_to_grid;
	}

	/*****************************************************************//**
	* Game Window Panel
	*********************************************************************/
	void LevelEditor::GameWindowPanel::onEvent(std::shared_ptr<Render::ViewportTexture> event) {
		texture_id = event->tex_id;
		event->setEventProcessed(true);
	}

	Vector2f LevelEditor::GameWindowPanel::getWorldMousePos() const {
		return world_mouse_pos;
	}

	Vector2f LevelEditor::GameWindowPanel::getWindowMousePos() const {
		return window_mouse_pos;
	}

	bool LevelEditor::GameWindowPanel::isMouseInWindow() const {
		//Get window size
		Vector2i window_size = NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize();

		//Check if relative cursor is within window size
		if (window_mouse_pos.x >= 0.0f &&
			window_mouse_pos.y >= 0.0f &&
			window_mouse_pos.x <= static_cast<float>(window_size.x) &&
			window_mouse_pos.y <= static_cast<float>(window_size.y))
		{
			return true;
		}
		else {
			return false;
		}
	}

	void LevelEditor::GameWindowPanel::init() {
		std::shared_ptr<GameWindowPanel> game_window_listener(this, [](GameWindowPanel*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Render::ViewportTexture>(game_window_listener);

		//Usage of tile map panel for rendering grid
		tile_map_panel = std::dynamic_pointer_cast<TileMapPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(TileMapPanel::getStaticName()));

		//Main panel reference
		main_panel = std::dynamic_pointer_cast<MainPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(MainPanel::getStaticName()));

		//Components panel reference
		comps_panel = std::dynamic_pointer_cast<ComponentsPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(ComponentsPanel::getStaticName()));
	}

	void LevelEditor::GameWindowPanel::update() {
	}

	void LevelEditor::GameWindowPanel::render()
	{
		ImGui::Begin(getName().c_str());

		//Get Imgui input
		ImGuiIO& io = ImGui::GetIO();

		//Get imgui draw list for custom rendering
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		//Actual Window Size
		Vector2f win_size = { static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x) , static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y) };

		//Configure viewport size
		ImVec2 window_size = ImGui::GetContentRegionAvail();
		float aspect_ratio = NIKE_WINDOWS_SERVICE->getWindow()->getAspectRatio();
		float viewport_width = window_size.x;
		float viewport_height = window_size.x / aspect_ratio;
		if (viewport_height > window_size.y) {
			viewport_height = window_size.y;
			viewport_width = window_size.y * aspect_ratio;
		}

		//Get the position of the game window top-left corner
		ImVec2 window_pos = ImGui::GetCursorScreenPos();

		//World Scale Factor
		Vector2f scale{ NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize().x / (viewport_width / NIKE_CAMERA_SERVICE->getActiveCamera().zoom),  NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize().y / (viewport_height / NIKE_CAMERA_SERVICE->getActiveCamera().zoom) };

		//Calculate world mouse position
		world_mouse_pos = { (io.MousePos.x - window_pos.x) * scale.x, (io.MousePos.y - window_pos.y) * scale.y };
		world_mouse_pos.x = world_mouse_pos.x - ((viewport_width * scale.x) / 2.0f) + NIKE_CAMERA_SERVICE->getActiveCamera().position.x;
		world_mouse_pos.y = world_mouse_pos.y - ((viewport_height * scale.y) / 2.0f) - NIKE_CAMERA_SERVICE->getActiveCamera().position.y;

		//Fixed window scale factor
		scale = { win_size.x / viewport_width, win_size.y / viewport_height };

		//Calculate window mouse position
		window_mouse_pos = { (io.MousePos.x - window_pos.x) * scale.x , (io.MousePos.y - window_pos.y) * scale.y };

		//Calculate UV coordinates for cropping when there are gaps
		Vector2f gaps = NIKE_WINDOWS_SERVICE->getWindow()->getViewportWindowGap();
		float u_min = gaps.x / 2.0f / win_size.x;
		float u_max = 1.0f - u_min;
		float v_min = gaps.y / 2.0f / win_size.y;
		float v_max = 1.0f - v_min;

		//Configure UV Offsets3fc
		ImVec2 uv0(u_min, -v_min); // Bottom-left
		ImVec2 uv1(u_max, -v_max); // Top-right

		//Render game to viewport
		ImGui::Image((ImTextureID)texture_id, ImVec2(viewport_width, viewport_height), uv0, uv1);

		//If grid is showing
		if (main_panel.lock()->getGridState()) {

			//Render grid
			tile_map_panel.lock()->renderGrid(draw_list, Vector2f(viewport_width, viewport_height));
		}

		//Render selected entity bounding box
		comps_panel.lock()->renderEntityBoundingBox(draw_list, Vector2f(viewport_width, viewport_height));

		//Check if gizmo state is active
		if (main_panel.lock()->getGizmoState()) {
			//Render selected entity gizmo
			comps_panel.lock()->renderEntityGizmo(draw_list, Vector2f(viewport_width, viewport_height));
		}

		ImGui::End();
	}
}
