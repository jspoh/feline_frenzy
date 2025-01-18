/*****************************************************************//**
 * \file   sEditorPanels.cpp
 * \brief  Level Editor panel implementation
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (70%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (15%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (15%)
 * \date   September 2024
 *  All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/LevelEditor/sEditorPanels.h"
#include "Core/Engine.h"
#include "Systems/Render/sysRender.h"
#include <ShlObj.h>

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
		popups.emplace(popup_id, PopUp{ false, std::move(popup_func) });
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
		return { window_pos.x + (render_size.x / 2.0f) + ((-NIKE_CAMERA_SERVICE->getActiveCamera().position.x + pos.x) * scale.x / NIKE_CAMERA_SERVICE->getActiveCamera().zoom),
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
						NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Audio::PausedEvent>(mode));
						};

					//Undo game mode
					set_game_state.undo_action = [&, mode = b_game_state]() {
						setGameState(mode);
						NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Audio::PausedEvent>(mode));
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

					//Restart scene
					NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::RESTART, ""));
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
	nlohmann::json LevelEditor::EntityMetaData::serialize() const {
		return	{
		{"Entity_ID", entity_id},
		{"Prefab_ID", prefab_id},
		{"B_Locked", b_locked}
		};
	}

	void LevelEditor::EntityMetaData::deserialize(nlohmann::json const& data) {
		entity_id = data["Entity_ID"].get<std::string>();
		prefab_id = data["Prefab_ID"].get<std::string>();
		b_locked = data["B_Locked"].get<bool>();
	}

	std::function<void()> LevelEditor::EntitiesPanel::createEntityPopUp(std::string const& popup_id) {
		return [this, popup_id]() {

			//Static entity name input buffer
			static std::string entity_name;

			//Get entity text
			ImGui::Text("Enter a name for the new entity:");
			if (ImGui::InputText("##Entity Name", entity_name.data(), entity_name.capacity() + 1)) {
				entity_name.resize(strlen(entity_name.c_str()));
			}

			//Static layer id
			static int layer_id = 0;

			//Get entity layer id
			ImGui::Text("Enter layer id for the new entity:");
			ImGui::InputInt("##EntityLayerIDInput", &layer_id, 1);

			//Clamp layer ID
			layer_id = std::clamp(layer_id, 0, std::clamp(static_cast<int>(NIKE_SCENES_SERVICE->getLayerCount() - 1), 0, 64));

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
					entities[new_id] = EntityMetaData(shared_id->c_str(), "", false);
					entity_to_name[new_id] = shared_id->c_str();
					name_to_entity[shared_id->c_str()] = new_id;
					};

				//Undo Action
				create.undo_action = [&, shared_id]() {

					//Check if entity is still alive
					if (name_to_entity.find(shared_id->data()) != name_to_entity.end()) {
						//Destroy new entity
						NIKE_ECS_MANAGER->destroyEntity(name_to_entity.at(shared_id->data()));
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

					//Save entity name into entities ref
					entities[new_id] = EntityMetaData(shared_id->c_str(), "", false);
					entity_to_name[new_id] = shared_id->c_str();
					name_to_entity[shared_id->c_str()] = new_id;

					//Set selected entity back to old entity
					selected_entity = name_to_entity.at(shared_id->data());
					};

				//Setup action for removing entity
				remove.do_action = [&, shared_id]() {

					//Check if entity is still alive
					if (name_to_entity.find(shared_id->data()) != name_to_entity.end()) {

						//Destroy entity
						NIKE_ECS_MANAGER->destroyEntity(name_to_entity.at(shared_id->data()));

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
						entities[new_id] = EntityMetaData(shared_id->c_str(), "", false);
						entity_to_name[new_id] = shared_id->c_str();
						name_to_entity[shared_id->c_str()] = new_id;
					}
					};

				//Undo Action
				clone.undo_action = [&, shared_id]() {

					//Check if entity is still alive
					if (name_to_entity.find(shared_id->data()) != name_to_entity.end()) {
						//Destroy new entity
						NIKE_ECS_MANAGER->destroyEntity(name_to_entity.at(shared_id->data()));
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

	void LevelEditor::EntitiesPanel::onEvent(std::shared_ptr<Coordinator::EntitiesChanged> event) {
		updateEntities(event->entities);
	}

	void LevelEditor::EntitiesPanel::init() {

		//Setup events listening
		std::shared_ptr<LevelEditor::EntitiesPanel> entitiespanel_wrapped(this, [](LevelEditor::EntitiesPanel*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Coordinator::EntitiesChanged>(entitiespanel_wrapped);

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
		int index = 0;
		for (auto it = entities.begin(); it != entities.end(); ++it) {

			//Update entities ref
			if (it->second.entity_id.find("entity_") != std::string::npos) {

				//Create identifier for entity
				char entity_name[32];
				snprintf(entity_name, sizeof(entity_name), "entity_%04d", index++);
				auto data = getEntityMetaData(it->first);
				data.entity_id = entity_name;
				setEntityMetaData(it->first, data);
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
			// Get entities marked for deletion
			auto entities_to_destroy = NIKE_ECS_MANAGER->getEntitiesToDestroy();

			//Check for disable entity interaction flag
			if (!checkPopUpShowing() && game_panel.lock()->isMouseInWindow() && !comp_panel.lock()->checkGizmoInteraction() && !tilemap_panel.lock()->checkGridEditing()) {
				//Reverse Iterate through layers to check for entity being clicked
				static bool entity_clicked = false;
				entity_clicked = false;

				//Iterate through entities from top layer down
				for (auto layer = NIKE_SCENES_SERVICE->getLayers().rbegin();
					!entity_clicked && layer != NIKE_SCENES_SERVICE->getLayers().rend();
					layer++) {

					//SKip inactive layer
					if (!layer->get()->getLayerState())
						continue;

					//Iterate through all entities
					for (auto& entity : entities) {

						//Skip entities not on curr layer
						if (layer->get()->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity.first))
							continue;

						// Check for entity clicking
						if (isCursorInEntity(entity.first) && ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left]) {
							// Only act if the entity changes
							if (selected_entity != entity.first) {

								LevelEditor::Action select_entity_action;

								// Capture current and previous selected entities
								auto prev_entity = selected_entity;
								auto new_entity = entity.first;

								// Define the do action for selecting the new entity
								select_entity_action.do_action = [&, prev_entity, new_entity]() {
									selected_entity = new_entity;
									b_entity_changed = true;
									entity_clicked = true;
									};

								// Define the undo action for reverting to the previous entity
								select_entity_action.undo_action = [&, prev_entity, new_entity]() {
									selected_entity = prev_entity;
									b_entity_changed = true;
									entity_clicked = true;
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(select_entity_action));
							}

							// Set the selected entity
							selected_entity = entity.first;

							// Get selected entity data
							auto it = entity_to_name.find(selected_entity);

							// Signal entity changed
							b_entity_changed = true;

							// Signal that an entity has been clicked
							entity_clicked = true;
							break;
						}

					}
				}

				if (!entity_clicked && ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left]) {

					// If selected entity is not invalid
					if (selected_entity != UINT16_MAX) {

						LevelEditor::Action unselect_entity_action;

						// Capture the current selected entity
						auto prev_entity = selected_entity;

						// Define the do action for unselecting
						unselect_entity_action.do_action = [&, prev_entity]() {
							unselectEntity();
							b_entity_changed = true;
							};

						// Define the undo action for reselecting the previous entity
						unselect_entity_action.undo_action = [&, prev_entity]() {
							selected_entity = prev_entity;
							b_entity_changed = true;
							};

						// Execute the action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(unselect_entity_action));
					}
				}
			
			}

			//Iterate through all entities to showcase active entities
			for (auto& entity : entities) {

				// Skip through "Prefab Master" entity
				if (entity.second.entity_id.find("Prefab Master") != std::string::npos) {
					continue;
				}

				//Check if entity is selected
				bool selected = (entities.find(selected_entity) != entities.end()) && entity_to_name.at(entity.first).c_str() == entity_to_name.at(selected_entity).c_str();

				// Show selectable
				if (ImGui::Selectable(entity_to_name.at(entity.first).c_str(), selected)) {
					// Check if currently editing grid
					if (tilemap_panel.lock()->checkGridEditing()) {
						error_msg->assign("Editing grid now, unable to select entity.");
						openPopUp("Error");
						unselectEntity();
						break;
					}

					// Prepare for redo/undo if the entity selection changes
					if (selected_entity != entity.first) {
						LevelEditor::Action select_entity_action;

						// Capture current and previous selected entities
						auto prev_entity = selected_entity;
						auto new_entity = entity.first;

						// Define the do action for selecting the new entity
						select_entity_action.do_action = [&, prev_entity, new_entity]() {
							selected_entity = new_entity;
							b_entity_changed = true;
							};

						// Define the undo action for reverting to the previous entity
						select_entity_action.undo_action = [&, prev_entity, new_entity]() {
							selected_entity = prev_entity;
							b_entity_changed = true;
							};

						// Execute the action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(select_entity_action));
					}
					// Unselect the entity
					else {
						LevelEditor::Action unselect_entity_action;

						// Capture the current selected entity
						auto prev_entity = selected_entity;

						// Define the do action for unselecting
						unselect_entity_action.do_action = [&, prev_entity]() {	
							unselectEntity();
							b_entity_changed = true;
							};

						// Define the undo action for reselecting the previous entity
						unselect_entity_action.undo_action = [&, prev_entity]() {
							selected_entity = prev_entity;
							b_entity_changed = true;
							};

						// Execute the action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(unselect_entity_action));
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

	void LevelEditor::EntitiesPanel::setEntityMetaData(Entity::Type entity, EntityMetaData data) {
		//Get selected entity data
		auto it = entities.find(entity);

		if (it == entities.end()) {
			throw std::runtime_error("Entity does not exist!");
		}

		entities[entity] = data;
		entity_to_name[entity] = data.entity_id;
		name_to_entity[data.entity_id] = entity;
	}

	LevelEditor::EntityMetaData LevelEditor::EntitiesPanel::getEntityMetaData(Entity::Type entity) const {
		//Get selected entity data
		auto it = entities.find(entity);

		if (it != entities.end()) {
			return entities.at(entity);
		}
		else {
			throw std::runtime_error("Entity does not exist!");
		}
	}

	std::optional<std::reference_wrapper<LevelEditor::EntityMetaData>> LevelEditor::EntitiesPanel::getSelectedEntityMetaData() {
		//Get selected entity data
		auto it = entities.find(selected_entity);

		if (it != entities.end()) {
			return it->second;
		}
		else {
			return std::nullopt;
		}
	}

	void LevelEditor::EntitiesPanel::lockEntity(Entity::Type entity) {
		//Get selected entity data
		auto it = entities.find(entity);

		if (it == entities.end()) {
			throw std::runtime_error("Entity does not exist!");
		}

		entities.at(entity).b_locked = true;
	}

	void LevelEditor::EntitiesPanel::lockAllEntities() {
		for (auto& entity : entities) {
			entity.second.b_locked = true;
		}
	}

	void LevelEditor::EntitiesPanel::unlockEntity(Entity::Type entity) {
		//Get selected entity data
		auto it = entities.find(entity);

		if (it == entities.end()) {
			throw std::runtime_error("Entity does not exist!");
		}

		entities.at(entity).b_locked = false;
	}

	void LevelEditor::EntitiesPanel::unlockAllEntities() {
		for (auto& entity : entities) {
			entity.second.b_locked = false;
		}
	}

	bool LevelEditor::EntitiesPanel::isEntityChanged() const {
		return b_entity_changed;
	}

	void LevelEditor::EntitiesPanel::updateEntities(std::set<Entity::Type> ecs_entities) {

		//Remove entities that are no longer in the ECS
		for (auto it = entities.begin(); it != entities.end();) {
			if (ecs_entities.find(it->first) == ecs_entities.end()) {
				//Remove the entity from all associated maps
				name_to_entity.erase(entity_to_name.at(it->first));
				entity_to_name.erase(it->first);
				it = entities.erase(it);
			}
			else {
				++it;
			}
		}

		//Add new entities from the ECS that are not yet in the editor
		for (auto& entity : ecs_entities) {

			if (entities.find(entity) == entities.end()) {

				//Create identifier for entity
				char entity_name[32];
				snprintf(entity_name, sizeof(entity_name), "entity_%04d", static_cast<int>(entity_to_name.size()));

				//Add entity to editor structures
				entities.emplace(entity, EntityMetaData(entity_name, "", false));
				entity_to_name.emplace(entity, entity_name);
				name_to_entity.emplace(entity_name, entity);
			}
		}
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
		if (!entities_panel.lock()->getSelectedEntityMetaData().has_value() || entities_panel.lock()->getSelectedEntityMetaData().value().get().b_locked) {
			return;
		}

		//Get transform component
		auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entities_panel.lock()->getSelectedEntity());

		if (!e_transform_comp.has_value())
			return;

		//Get transform
		auto& e_transform = e_transform_comp.value().get();

		//Get camera zoom
		float cam_zoom = NIKE_CAMERA_SERVICE->getActiveCamera().zoom;

		//Get gizmo Scale
		float gizmo_scale = gizmo.gizmo_scaling * cam_zoom;

		//Get mouse pos
		Vector2f world_mouse = game_panel.lock()->getWorldMousePos();

		//Render for each gizmo mode
		switch (gizmo.mode) {
		case GizmoMode::Translate: {
			//Extra object for translate ( Move box )
			gizmo.objects["Move Box"].first.position = { e_transform.position.x + gizmo_scale * 2.5f + gizmo.x_axis_offset * cam_zoom, e_transform.position.y + gizmo_scale * 2.5f + gizmo.y_axis_offset * cam_zoom };
			gizmo.objects["Move Box"].first.scale = { gizmo_scale, gizmo_scale };
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
				e_transform.position = { world_mouse.x - (gizmo_scale * 2.5f + gizmo.x_axis_offset * cam_zoom),  -world_mouse.y - (gizmo_scale * 2.5f + gizmo.y_axis_offset * cam_zoom) };
			}

			//Add gizmo up
			gizmo.objects["Up"].first.position = { e_transform.position.x, e_transform.position.y + gizmo.y_axis_offset * cam_zoom };
			gizmo.objects["Up"].first.scale = { gizmo_scale * 0.3f , gizmo_scale * 4 };
			gizmo.objects["Up"].second = { 0, 255, 0, 255 };

			//Add gizmo up point
			gizmo.objects["Up Point"].first.position = { e_transform.position.x, e_transform.position.y + gizmo_scale * 2.5f + gizmo.y_axis_offset * cam_zoom };
			gizmo.objects["Up Point"].first.scale = { gizmo_scale * 0.7f, gizmo_scale };
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
			gizmo.objects["Right"].first.position = { e_transform.position.x + gizmo.x_axis_offset * cam_zoom , e_transform.position.y };
			gizmo.objects["Right"].first.scale = { gizmo_scale * 4, gizmo_scale * 0.3f };
			gizmo.objects["Right"].second = { 255, 0, 0, 255 };

			//Add gizmo right point
			gizmo.objects["Right Point"].first.position = { e_transform.position.x + gizmo_scale * 2.5f + gizmo.x_axis_offset * cam_zoom, e_transform.position.y };
			gizmo.objects["Right Point"].first.scale = { gizmo_scale, gizmo_scale * 0.7f };
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
			//gizmo.objects["Center"].first.position = { e_transform.position.x, e_transform.position.y };
			//gizmo.objects["Center"].first.scale = { gizmo_scale, gizmo_scale };
			//gizmo.objects["Center"].second = { 255, 255, 255, 255 };

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
			gizmo.objects["Up"].first.position = { e_transform.position.x, e_transform.position.y + gizmo.y_axis_offset * cam_zoom };
			gizmo.objects["Up"].first.scale = { gizmo_scale * 0.3f , gizmo_scale * 4 };
			gizmo.objects["Up"].second = { 0, 255, 0, 255 };

			//Add gizmo up point
			gizmo.objects["Up Point"].first.position = { e_transform.position.x, e_transform.position.y + gizmo_scale * 2.f + gizmo.y_axis_offset * cam_zoom };
			gizmo.objects["Up Point"].first.scale = { gizmo_scale * 0.8f, gizmo_scale * 0.8f };
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
			gizmo.objects["Right"].first.position = { e_transform.position.x + gizmo.x_axis_offset * cam_zoom, e_transform.position.y };
			gizmo.objects["Right"].first.scale = { gizmo_scale * 4, gizmo_scale * 0.3f };
			gizmo.objects["Right"].second = { 255, 0, 0, 255 };

			//Add gizmo right point
			gizmo.objects["Right Point"].first.position = { e_transform.position.x + gizmo_scale * 2.f + gizmo.x_axis_offset * cam_zoom, e_transform.position.y };
			gizmo.objects["Right Point"].first.scale = { gizmo_scale * 0.8f, gizmo_scale * 0.8f };
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
			//gizmo.objects["Center"].first.position = { e_transform.position.x, e_transform.position.y };
			//gizmo.objects["Center"].first.scale = { e_transform.scale.x * 0.25f, e_transform.scale.y * 0.25f };
			//gizmo.objects["Center"].second = { 255, 255, 255, 255 };

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
			gizmo.objects["Rot Circle"].first.scale = { gizmo_scale * 8, gizmo_scale * 8 };
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
				gizmo.objects["Rot Circle"].first.position.x + (gizmo_scale * 4) * cos(e_transform.rotation * static_cast<float>(M_PI) / 180.0f),
				gizmo.objects["Rot Circle"].first.position.y + (gizmo_scale * 4) * sin(e_transform.rotation * static_cast<float>(M_PI) / 180.0f)
			};
			gizmo.objects["Rot Point"].first.scale = { gizmo_scale * 0.8f, gizmo_scale * 0.8f };
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
					//Action add_comp;

					//Setup undo action for add component
					//add_comp.undo_action = [=]() {
					//	if (NIKE_ECS_MANAGER->checkEntityComponent(entities_panel.lock()->getSelectedEntity(), component.second)) {
					//		NIKE_ECS_MANAGER->removeEntityComponent(entities_panel.lock()->getSelectedEntity(), component.second);
					//	}
					//};

					//Setup do action for add component
					//add_comp.do_action = [=]() {

						//Add default comp to entity
					NIKE_ECS_MANAGER->addDefEntityComponent(entities_panel.lock()->getSelectedEntity(), component.second);
					//	};

					//Execute add component action
					//NIKE_LVLEDITOR_SERVICE->executeAction(std::move(add_comp));

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
			layer_id = std::clamp(layer_id, 0, std::clamp(static_cast<int>(NIKE_SCENES_SERVICE->getLayerCount() - 1), 0, 64));

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

	std::function<void()> LevelEditor::ComponentsPanel::removeComponentPopUp(std::string const& popup_id)
	{
		return [this, popup_id] {
			// Warning message
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "This action cannot be undone!");

			// Confirmation message
			ImGui::Text("Are you sure you want to remove the component?");
			ImGui::Spacing();

			// Confirm button
			if (ImGui::Button("Ok")) {
				// Retrieve component type from reference
				Component::Type comp_type_copy = comps.at(comp_string_ref);

				// Remove the component from the entity
				NIKE_ECS_MANAGER->removeEntityComponent(entities_panel.lock()->getSelectedEntity(), comp_type_copy);

				//Reset comp string ref
				comp_string_ref.clear();

				// Close popup
				closePopUp(popup_id);
			}

			ImGui::SameLine();

			// Cancel button
			if (ImGui::Button("Cancel")) {
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
		registerPopUp("Remove Component", removeComponentPopUp("Remove Component"));
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

	void LevelEditor::ComponentsPanel::setCompStringRef(std::string const& to_set)
	{
		comp_string_ref = to_set;
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
				if (entities_panel.lock()->getSelectedEntityMetaData().has_value()) {
					auto& locked = entities_panel.lock()->getSelectedEntityMetaData().value().get().b_locked;
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

					ImGui::Spacing();

					static bool show_gizmo_settings = true; // Checkbox state to toggle gizmo settings

					// Checkbox to enable or disable gizmo settings
					ImGui::Checkbox("Show Gizmo Settings", &show_gizmo_settings);

					// Display gizmo settings only if the checkbox is enabled
					if (show_gizmo_settings) {
						// Set Gizmo Sensitivity
						{
							ImGui::Text("Set Gizmo Sensitivity: ");
							ImGui::DragFloat("##GizmoSensitivity", &gizmo.sensitivity, 0.01f, 0.0f, 5.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

							// Before editing
							static float sensitivity_before_change = gizmo.sensitivity;
							if (ImGui::IsItemActivated()) {
								sensitivity_before_change = gizmo.sensitivity;
							}

							// Upon editing finish
							if (ImGui::IsItemDeactivatedAfterEdit()) {
								Action set_gizmo_sensi;

								// Do gizmo sensitivity
								set_gizmo_sensi.do_action = [&, sensi = gizmo.sensitivity]() {
									gizmo.sensitivity = sensi;
									};

								// Undo gizmo sensitivity
								set_gizmo_sensi.undo_action = [&, sensi = sensitivity_before_change]() {
									gizmo.sensitivity = sensi;
									};

								// Execute action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_gizmo_sensi));
							}
						}

						// Set Gizmo Scale
						{
							ImGui::Text("Set Gizmo Scale: ");
							ImGui::DragFloat("##GizmoScale", &gizmo.gizmo_scaling, 0.1f, 0.1f, (float)UINT16_MAX, "%.1f", ImGuiSliderFlags_AlwaysClamp);

							static float scale_before_change = gizmo.gizmo_scaling;

							if (ImGui::IsItemActivated()) {
								scale_before_change = gizmo.gizmo_scaling;
							}

							if (ImGui::IsItemDeactivatedAfterEdit()) {
								Action set_gizmo_scale;

								set_gizmo_scale.do_action = [&, scale = gizmo.gizmo_scaling]() {
									gizmo.gizmo_scaling = scale;
									};

								set_gizmo_scale.undo_action = [&, scale = scale_before_change]() {
									gizmo.gizmo_scaling = scale;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_gizmo_scale));
							}
						}

						// Set X Axis Offset
						{
							ImGui::Text("Set X Axis Offset: ");
							ImGui::DragFloat("##GizmoXAxis", &gizmo.x_axis_offset, 0.01f, 0.0f, (float)UINT16_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);

							static float x_offset_before_change = gizmo.x_axis_offset;

							if (ImGui::IsItemActivated()) {
								x_offset_before_change = gizmo.x_axis_offset;
							}

							if (ImGui::IsItemDeactivatedAfterEdit()) {
								Action set_x_offset;

								set_x_offset.do_action = [&, offset = gizmo.x_axis_offset]() {
									gizmo.x_axis_offset = offset;
									};

								set_x_offset.undo_action = [&, offset = x_offset_before_change]() {
									gizmo.x_axis_offset = offset;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_x_offset));
							}
						}

						// Set Y Axis Offset
						{
							ImGui::Text("Set Y Axis Offset: ");
							ImGui::DragFloat("##GizmoYAxis", &gizmo.y_axis_offset, 0.01f, 0.0f, (float)UINT16_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);

							static float y_offset_before_change = gizmo.y_axis_offset;

							if (ImGui::IsItemActivated()) {
								y_offset_before_change = gizmo.y_axis_offset;
							}

							if (ImGui::IsItemDeactivatedAfterEdit()) {
								Action set_y_offset;

								set_y_offset.do_action = [&, offset = gizmo.y_axis_offset]() {
									gizmo.y_axis_offset = offset;
									};

								set_y_offset.undo_action = [&, offset = y_offset_before_change]() {
									gizmo.y_axis_offset = offset;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_y_offset));
							}
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
						comp_string_ref = comp.first;
						openPopUp("Remove Component");
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

	std::unordered_map<std::string, std::function<void(LevelEditor::ComponentsPanel&, void*)>>& LevelEditor::ComponentsPanel::getCompsUI()
	{
		return comps_ui;
	}

	std::unordered_map<std::string, Component::Type> LevelEditor::ComponentsPanel::getComps() const {
		return comps;
	}

	void LevelEditor::ComponentsPanel::renderEntityBoundingBox(void* draw_list, Vector2f const& render_size) {
		//Check if entity is locked
		if (!entities_panel.lock()->getSelectedEntityMetaData().has_value() || entities_panel.lock()->getSelectedEntityMetaData().value().get().b_locked) {
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
			worldCircle(draw, rotation_circle.first, rendersize, IM_COL32(rotation_circle.second.r, rotation_circle.second.g, rotation_circle.second.b, rotation_circle.second.a), gizmo.gizmo_scaling * 0.15f);
		}
		else {
			//Render rotated rectangle
			worldQuad(draw, e_transform, rendersize, IM_COL32(255, 255, 255, 255), (gizmo.gizmo_scaling * 0.05f));
		}
	}

	void LevelEditor::ComponentsPanel::renderEntityGizmo(void* draw_list, Vector2f const& render_size) {
		//Check if entity is locked
		if (!entities_panel.lock()->getSelectedEntityMetaData().has_value() || entities_panel.lock()->getSelectedEntityMetaData().value().get().b_locked) {
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
			//auto const& center = gizmo.objects["Center"];
			//worldCircleFilled(draw, center.first, rendersize, IM_COL32(center.second.r, center.second.g, center.second.b, center.second.a));

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
			worldLine(draw, e_transform.position, rotation_point.first.position, rendersize, IM_COL32(255, 255, 255, 255), gizmo.gizmo_scaling * 0.15f);

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
	* Prefab Management Panel
	*********************************************************************/
	std::function<void()> LevelEditor::PrefabsPanel::addComponentPopUp(std::string const& popup_id) {
		return [this, popup_id]() {

			//Select a component to add
			ImGui::Text("Select a component to add:");

			//Add spacing before components
			ImGui::Spacing();

			//Iterate over all registered components
			for (const auto& component : comps_panel.lock()->getComps()) {

				//Check if component already exists
				if (NIKE_ECS_MANAGER->checkEntityComponent(prefab_display, component.second))
					continue;

				//Display each component as a button
				if (ImGui::Button(component.first.c_str())) {

					//Add default comp to entity
					NIKE_ECS_MANAGER->addDefEntityComponent(prefab_display, component.second);

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

	std::function<void()> LevelEditor::PrefabsPanel::removeComponentPopUp(std::string const& popup_id) {
		return [this, popup_id] {
			// Warning message
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "This action cannot be undone!");

			// Confirmation message
			ImGui::Text("Are you sure you want to remove the component?##Prefab");
			ImGui::Spacing();

			// Confirm button
			if (ImGui::Button("Ok##Prefab")) {
				// Retrieve component type from reference
				Component::Type comp_type_copy = comps_panel.lock()->getComps().at(comp_ref);

				// Remove the component from the entity
				NIKE_ECS_MANAGER->removeEntityComponent(prefab_display, comp_type_copy);

				//Reset comp string ref
				comp_ref.clear();

				// Close popup
				closePopUp(popup_id);
			}

			ImGui::SameLine();

			// Cancel button
			if (ImGui::Button("Cancel##Prefab")) {
				closePopUp(popup_id);
			}
			};
	}

	std::function<void()> LevelEditor::PrefabsPanel::createPrefabPopUp(std::string const& popup_id)
	{
		return [this, popup_id]() {

			// Warning message
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "You are creating an empty prefab, remember to save it later!");

			//Static entity name input buffer
			static std::string prefab_id;

			//Get Scene text
			ImGui::Text("Enter a name for the prefab without .prefab:");
			if (ImGui::InputText("##prefab Name", prefab_id.data(), prefab_id.capacity() + 1)) {
				prefab_id.resize(strlen(prefab_id.c_str()));
			}

			//Add spacing
			ImGui::Spacing();

			//Display each component as a button
			if (ImGui::Button("Ok") && !prefab_id.empty() && (prefab_id.find(".prefab") == prefab_id.npos) && !NIKE_ASSETS_SERVICE->isAssetRegistered(std::string(prefab_id + ".prefab"))) {

				//Craft file path from name
				std::filesystem::path path = NIKE_PATH_SERVICE->resolvePath("Game_Assets:/Prefabs");
				if (std::filesystem::exists(path)) {
					path /= std::string(prefab_id + ".prefab");
				}
				else {
					path = NIKE_PATH_SERVICE->resolvePath("Game_Assets:/");
					path /= std::string(prefab_id + ".prefab");
				}

				//Normalize prefab path
				prefab_path = NIKE_PATH_SERVICE->normalizePath(path);

				//Path found, but create a temp entity
				createDisplayPrefab("");

				//Reset scene id buffer
				prefab_id.clear();

				//Close popup
				closePopUp(popup_id);
			}

			//Same line
			ImGui::SameLine();

			//Cancel deleting asset
			if (ImGui::Button("Cancel")) {

				//Reset scene id buffer
				prefab_id.clear();

				//Close popup
				closePopUp(popup_id);
			}
			};
	}

	std::function<void()> LevelEditor::PrefabsPanel::loadPrefabPopUp(std::string const& popup_id)
	{
		return [this, popup_id]
			{
				// Display header
				ImGui::Text("Select a Prefab to Load:");

				// Retrieve all loaded prefab files
				const auto& all_loaded_prefabs = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Prefab);

				// Track the selected prefab
				static int current_index = -1;

				// Display a combo box for prefab selection
				if (ImGui::Combo("##SelectPrefab", &current_index, all_loaded_prefabs.data(), static_cast<int>(all_loaded_prefabs.size()))) {
					// Clear any temporary prefab entity before creating a new one
					if (current_index >= 0 && current_index < static_cast<int>(all_loaded_prefabs.size())) {
						std::string selected_prefab_path = all_loaded_prefabs[current_index];

						//Create prefab
						createDisplayPrefab(selected_prefab_path);

						//Reset current index
						current_index = -1;

						// Close the popup after loading
						closePopUp(popup_id);
					}
				}

				// Cancel button
				if (ImGui::Button("Cancel")) {
					closePopUp(popup_id);
				}
			};
	}

	std::function<void()> LevelEditor::PrefabsPanel::createEntityPopup(std::string const& popup_id) {
		return [this, popup_id]() {

			//Static entity name input buffer
			static std::string entity_name;

			//Get entity text
			ImGui::Text("Enter a name for the new entity:");
			if (ImGui::InputText("##PrefabEntity Name", entity_name.data(), entity_name.capacity() + 1)) {
				entity_name.resize(strlen(entity_name.c_str()));
			}

			//Static layer id
			static int layer_id = 0;

			//Get entity layer id
			ImGui::Text("Enter layer id for the new entity:");
			ImGui::InputInt("##PrefabEntityLayerIDInput", &layer_id, 1);

			//Clamp layer ID
			layer_id = std::clamp(layer_id, 0, std::clamp(static_cast<int>(NIKE_SCENES_SERVICE->getLayerCount() - 1), 0, 64));

			//If enter or ok button is pressed
			if (ImGui::Button("OK") || ImGui::GetIO().KeysDown[NIKE_KEY_ENTER]) {

				//If entity name is not provided (Create a default)
				if (entity_name.empty())
				{
					snprintf(entity_name.data(), entity_name.capacity() + 1, "entity_%04d", NIKE_ECS_MANAGER->getEntitiesCount());
				}

				//Create new entity
				Entity::Type new_id = NIKE_ECS_MANAGER->createEntity(layer_id);

				//Load entity from path
				NIKE_SERIALIZE_SERVICE->loadEntityFromFile(new_id, prefab_path.string());

				//Add metadata to entity
				EntityMetaData data(entity_name.c_str(), prefab_path.filename().string(), false);
				entities_panel.lock()->setEntityMetaData(new_id, data);

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

	void LevelEditor::PrefabsPanel::savePrefab() {
		//Save prefab to file
		NIKE_SERIALIZE_SERVICE->saveEntityToFile(prefab_display, prefab_path.string());
	}

	void LevelEditor::PrefabsPanel::init() {
		//Components panel reference
		comps_panel = std::dynamic_pointer_cast<ComponentsPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(ComponentsPanel::getStaticName()));

		// Editor entities panel ref
		entities_panel = std::dynamic_pointer_cast<EntitiesPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(EntitiesPanel::getStaticName()));

		//Popups registration
		registerPopUp("Add Component", addComponentPopUp("Add Component"));
		registerPopUp("Remove Component", removeComponentPopUp("Remove Component"));
		registerPopUp("Load Prefab", loadPrefabPopUp("Load Prefab"));
		registerPopUp("Create Prefab", createPrefabPopUp("Create Prefab"));
		registerPopUp("Create Entity", createEntityPopup("Create Entity"));

		// Default pop up for saving prefab
		msg = std::make_shared<std::string>("Prefab Saved!");
		registerPopUp("Success", defPopUp("Success", msg));
	}

	void LevelEditor::PrefabsPanel::update() {
		copy_count = 0;
		for (auto const& entity : NIKE_ECS_MANAGER->getAllEntities()) {
			auto const& data = entities_panel.lock()->getEntityMetaData(entity);
			if (data.prefab_id == prefab_path.filename().string()) {
				copy_count++;
			}
		}
	}

	void LevelEditor::PrefabsPanel::render() {
		ImGui::Begin(getName().c_str());

		//Dummy For accepting payload
		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::Dummy(ImGui::GetContentRegionAvail());

		//Accept payload here
		prefabAcceptPayload();
		ImGui::SetCursorPos(pos);

		//Check if prefab display is valid
		if (prefab_display != UINT16_MAX && NIKE_ECS_MANAGER->checkEntity(prefab_display)) {

			//Set flag to true
			b_editing_prefab = true;

			//Display prefab ID
			ImGui::Text("Editing Prefab ID: %s", prefab_path.filename().string().c_str());
			ImGui::Text("Prefab's Copies: %i", copy_count);

			//Load prefab template
			if (ImGui::Button("Load Prefab")) {
				openPopUp("Load Prefab");
			}

			ImGui::SameLine();

			//Save prefab template
			if (ImGui::Button("Save Prefab")) {
				savePrefab();

				//Open success popup
				msg->assign("Prefab Saved!");
				openPopUp("Success");
			}

			ImGui::SameLine();

			//Save prefab template
			if (ImGui::Button("Close Prefab")) {
				destroyDisplayPrefab();
			}

			ImGui::Spacing();

			ImGui::Separator();

			ImGui::Spacing();

			ImGui::Text("Modify Prefab Template");

			//Add component
			if (ImGui::Button("Add Component")) {
				openPopUp("Add Component");
			}

			ImGui::SameLine();

			//Add component
			if (ImGui::Button("Create Entity")) {
				savePrefab();
				openPopUp("Create Entity");
			}

			ImGui::SameLine();

			//Add component
			if (ImGui::Button("Apply To Entities")) {

				//Save prefab
				savePrefab();

				//Iterate through all entities and check their metadata
				for (auto const& entity : NIKE_ECS_MANAGER->getAllEntities()) {
					auto const& data = entities_panel.lock()->getEntityMetaData(entity);

					//Apply prefab to all entities
					if (data.prefab_id == prefab_path.filename().string()) {

						//Check for transform
						auto e_trans_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
						if (e_trans_comp.has_value()) {

							//Save old position
							auto& e_transform = e_trans_comp.value().get();
							Vector2f prev_pos = e_transform.position;

							//Load entity
							NIKE_SERIALIZE_SERVICE->loadEntityFromFile(entity, prefab_path.string());

							//Apply old position back
							e_transform.position = prev_pos;
						}
						else {
							//Load entity
							NIKE_SERIALIZE_SERVICE->loadEntityFromFile(entity, prefab_path.string());
						}
					}
				}
			}

			ImGui::Spacing();

			ImGui::Separator();

			ImGui::Spacing();

			//Render prefab components
			renderPrefabComponents();
		}
		else {
			//Set flag to false
			b_editing_prefab = false;

			ImGui::Text("Drag or load a prefab here to start editing.");

			//Load prefab template
			if (ImGui::Button("Load Prefab")) {
				openPopUp("Load Prefab");
			}

			ImGui::SameLine();

			//Create empty prefab template
			if (ImGui::Button("Create Prefab")) {
				openPopUp("Create Prefab");
			}
		}

		renderPopUps();

		ImGui::End();
	}

	void LevelEditor::PrefabsPanel::prefabAcceptPayload()
	{
		if (ImGui::BeginDragDropTarget()) {

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Prefab_FILE")) {
				//Get asset ID
				std::string asset_id(static_cast<const char*>(payload->Data));

				createDisplayPrefab(asset_id);
			}

			ImGui::EndDragDropTarget();
		}
	}

	void LevelEditor::PrefabsPanel::renderPrefabComponents()
	{
		// Iterate through components of the entity
		for (auto& comp : NIKE_ECS_MANAGER->getAllEntityComponents(prefab_display)) {

			// Display a collapsible header for each component
			if (ImGui::CollapsingHeader(comp.first.c_str(), ImGuiTreeNodeFlags_None)) {
				// Use the component's UI rendering logic
				comps_panel.lock()->getCompsUI().at(comp.first)(*comps_panel.lock(), comp.second.get());

				// For removing component purpose
				comp_ref = comp.first;

				// Add a button to remove the component
				if (ImGui::Button(std::string("Remove Prefab Component##" + comp.first).c_str())) {
					openPopUp("Remove Component");
				}
			}
		}
	}

	void LevelEditor::PrefabsPanel::createDisplayPrefab(const std::string& file_path)
	{
		//Destroy previous master copy
		if (prefab_display != UINT16_MAX && NIKE_ECS_MANAGER->checkEntity(prefab_display)) {
			destroyDisplayPrefab();
		}

		// Create a temporary entity (excluded from systems)
		prefab_display = NIKE_ECS_MANAGER->createEntity();

		//Update entities list with master name
		EntityMetaData temp = entities_panel.lock()->getEntityMetaData(prefab_display);
		temp.entity_id = "Prefab Master";
		entities_panel.lock()->setEntityMetaData(prefab_display, temp);

		//Return if file path is empty
		if (file_path.empty()) {
			return;
		}

		//Load new entity
		prefab_path = NIKE_ASSETS_SERVICE->getAssetPath(file_path);
		NIKE_SERIALIZE_SERVICE->loadEntityFromFile(prefab_display, prefab_path.string());
	}

	void LevelEditor::PrefabsPanel::destroyDisplayPrefab()
	{
		//Destroy entity
		NIKE_ECS_MANAGER->destroyEntity(prefab_display);

		//Set prefab display to invalid value
		prefab_display = UINT16_MAX;

		//Reset prefab path
		prefab_path = "";
	}

	bool LevelEditor::PrefabsPanel::isPrefabEditing() const {
		return b_editing_prefab;
	}

	/*****************************************************************//**
	* Debugging Tools Panel
	*********************************************************************/
	void LevelEditor::DebugPanel::init() {

	}

	void LevelEditor::DebugPanel::update() {

	}

	void LevelEditor::DebugPanel::render() {
		if (!ImGui::Begin(getName().c_str())) {
			//Return if window is not being shown
			ImGui::End();
			return;
		}

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

				static char documents_path[MAX_PATH] = "";

				// Get the path to the Desktop folder
				if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documents_path) != S_OK) {
					cerr << "Failed to get desktop path!" << endl;
				}

				// Open crash log file
				std::ifstream crashLogFile(std::string{ documents_path } + R"(\feline-frenzy-logs\crash-log.txt)");

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
	* Audio Management Panel
	*********************************************************************/

	std::function<void()> LevelEditor::AudioPanel::createChannelPopUp(std::string const& popup_id) {
		return [this, popup_id]() {
			//Static channel name input buffer
			static std::string channel_name;
			static bool valid_name = true;

			//Get entity text
			ImGui::Text("Enter a name for the new channel:");
			if (ImGui::InputText("##Entity Name", channel_name.data(), channel_name.capacity() + 1)) {
				channel_name.resize(strlen(channel_name.c_str()));
			}
			if (!valid_name) {
				//Warning message
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please enter a valid name!");
			}

			//If enter or ok button is pressed
			if (ImGui::Button("OK")) {
				//Temporary create action
				Action create;

				//Create a shared id for do & undo functions
				std::shared_ptr<std::string> shared_id = std::make_shared<std::string>(channel_name);

				//If entity name is not provided (Create a default)
				if (shared_id->empty() || NIKE_AUDIO_SERVICE->checkChannelGroupExist(channel_name))
				{
					valid_name = false;
				}
				else {

					//Do Action
					create.do_action = [&, shared_id]() {
						//Create new channel 
						if (!NIKE_AUDIO_SERVICE->checkChannelGroupExist(shared_id->c_str())) {
							NIKE_AUDIO_SERVICE->createChannelGroup(shared_id->c_str());
						}
						};

					//Undo Action
					create.undo_action = [&, shared_id]() {

						//Check if channel exists
						if (NIKE_AUDIO_SERVICE->checkChannelGroupExist(shared_id->c_str())) {
							//unload channel
							NIKE_AUDIO_SERVICE->unloadChannelGroup(shared_id->c_str());
						}
						};

					//Execute create entity action
					NIKE_LVLEDITOR_SERVICE->executeAction(std::move(create));

					//Reset channel name
					channel_name.clear();
					valid_name = true;


					//Close popup
					closePopUp(popup_id);
				}
			}

			ImGui::SameLine();
			//Cancel creating new entity
			if (ImGui::Button("Cancel")) {

				//Reset entity name
				channel_name.clear();
				valid_name = true;

				//Close popup
				closePopUp(popup_id);
			}
			};
	}

	std::function<void()> LevelEditor::AudioPanel::deleteChannelPopUp(std::string const& popup_id) {

		return [this, popup_id] {

			//Select a component to add
			ImGui::Text("Are you sure you want to delete channel: %s?", selected_channel_name.c_str());

			//Add spacing
			ImGui::Spacing();

			//Display each component as a button
			if (ImGui::Button("Confirm")) {

				Action remove;

				std::shared_ptr<std::string> shared_id = std::make_shared<std::string>(selected_channel_name.c_str());
				remove.undo_action = [&, shared_id]() {
					//Create new channel 
					if (!NIKE_AUDIO_SERVICE->checkChannelGroupExist(shared_id->c_str())) {
						NIKE_AUDIO_SERVICE->createChannelGroup(shared_id->c_str());
					}
					};

				remove.do_action = [&, shared_id]() {
					if (NIKE_AUDIO_SERVICE->checkChannelGroupExist(shared_id->c_str())) {
						NIKE_AUDIO_SERVICE->unloadChannelGroup(shared_id->c_str());
					}
					else {
						NIKEE_CORE_ERROR("Error: Unable to delete channel group!");
					}
					};

				//Execute remove action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(remove));

				selected_channel_name.clear();
				//Close popup
				closePopUp(popup_id);
			}

			//Same line
			ImGui::SameLine();

			//Cancel deleting asset
			if (ImGui::Button("Cancel")) {

				//Close popup
				closePopUp(popup_id);
			}
			};

	}

	void LevelEditor::AudioPanel::init() {
		registerPopUp("Add New Channel", createChannelPopUp("Add New Channel"));
		registerPopUp("Delete Channel", deleteChannelPopUp("Delete Channel"));
	}

	void LevelEditor::AudioPanel::update() {

	}

	void LevelEditor::AudioPanel::render() {
		ImGui::Begin(getName().c_str());

		if (ImGui::Button("Add New Channel")) {
			openPopUp("Add New Channel");
		}

		ImGui::Separator();

		static std::unordered_map<std::string, std::string> channel_audio_ids;
		static std::unordered_map<std::string, int> channel_indices;
		static std::unordered_map<std::string, bool> open_playlists;

		const auto& all_loaded_sounds = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Music);

		for (auto& channel : NIKE_AUDIO_SERVICE->getAllChannelGroups()) {
			ImGui::Spacing();

			if (ImGui::CollapsingHeader(std::string("Channel: " + channel.first).c_str(), ImGuiTreeNodeFlags_None)) {

				ImGui::Spacing();

				float volume = channel.second->getVolume();
				float pitch = channel.second->getPitch();

				ImGui::Text("Adjust Volume & Pitch");
				if (ImGui::SliderFloat(std::string("Volume##" + channel.first).c_str(), &volume, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
					channel.second->setVolume(volume);
				}
				if (ImGui::SliderFloat(std::string("Pitch##" + channel.first).c_str(), &pitch, 0.5f, 2.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
					channel.second->setPitch(pitch);
				}

				ImGui::Spacing();


				if (ImGui::Button(channel.second->getPaused() ? std::string("Unpause Channel##" + channel.first).c_str() : std::string("Pause Channel##" + channel.first).c_str())) {
					channel.second->setPaused(!channel.second->getPaused());
				}

				ImGui::SameLine();

				if (ImGui::Button(std::string("Stop Audio In Channel##" + channel.first).c_str())) {
					channel.second->stop();
				}


				ImGui::Spacing();

				if (ImGui::Button((std::string("Delete Channel##" + channel.first).c_str()))) {
					selected_channel_name = channel.first;
					openPopUp("Delete Channel");
				}

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				ImGui::Text("Playlist Management");
				ImGui::Spacing();

				if (open_playlists.find(channel.first) == open_playlists.end()) {
					open_playlists[channel.first] = false;
				}
				bool& open_playlist = open_playlists[channel.first];

				ImGui::Checkbox(std::string("Open Playlist##" + channel.first).c_str(), &open_playlists[channel.first]);

				ImGui::Spacing();

				if (open_playlist) {
					bool loop_playlist = NIKE_AUDIO_SERVICE->isPlaylistLooping(channel.first);
					// Get channel audio id and index from map
					if (channel_audio_ids.find(channel.first) == channel_audio_ids.end()) {
						channel_audio_ids[channel.first] = "";
					}
					if (channel_indices.find(channel.first) == channel_indices.end()) {
						channel_indices[channel.first] = -1;
					}

					std::string& current_audio_id = channel_audio_ids[channel.first];
					int& current_index = channel_indices[channel.first];

					ImGui::Spacing();

					if (current_index == -1) {
						auto it = std::find(all_loaded_sounds.begin(), all_loaded_sounds.end(), current_audio_id);
						current_index = (it != all_loaded_sounds.end() ? (int)std::distance(all_loaded_sounds.begin(), it) : -1);
					}

					ImGui::Text("Select audio to add to queue:");
					if (ImGui::Combo(std::string("##SELECTAUDIO_" + channel.first).c_str(), &current_index, all_loaded_sounds.data(), static_cast<int>(all_loaded_sounds.size()))) {
						if (current_index >= 0 && current_index < all_loaded_sounds.size()) {
							current_audio_id = all_loaded_sounds[current_index];
						}
					}

					ImGui::SameLine();

					//Save Shape model ID Button
					if (ImGui::Button(std::string("Add##ADDAUDIOBTN_" + channel.first).c_str())) {
						if (NIKE_ASSETS_SERVICE->isAssetRegistered(current_audio_id)) {
							//LevelEditor::Action save;

							//save.do_action = [&, shared_id]() {

							//};


							//save.undo_action = [&, shared_id]() {

							//};
							NIKE_AUDIO_SERVICE->queueAudioToPlaylist(channel.first, current_audio_id);
						}
					}

					ImGui::Spacing();


					if (ImGui::Button((std::string("Clear Playlist##" + channel.first).c_str()))) {
						NIKE_AUDIO_SERVICE->clearPlaylist(channel.first);
					}

					ImGui::SameLine();

					if (ImGui::Checkbox(std::string("Loop Playlist##" + channel.first).c_str(), &loop_playlist)) {
						NIKE_AUDIO_SERVICE->setPlaylistLoop(channel.first, loop_playlist);
					}

					const auto& playlist = NIKE_AUDIO_SERVICE->getChannelPlaylist(channel.first);


					ImGui::Spacing();

					ImGui::Text(std::string(channel.first + " Playlist:").c_str());
					if (playlist.tracks.empty()) {
						ImGui::BulletText("No audio queued");
					}
					else {
						for (const auto& track : playlist.tracks) {
							ImGui::BulletText("%s", track.c_str());
						}
					}

					ImGui::Spacing();
				}
				ImGui::Spacing();
				ImGui::Separator();
			}

		}
		// Render pop-ups
		renderPopUps();
		ImGui::End();
	}

	/*****************************************************************//**
	* Resource Management Panel
	*********************************************************************/
	void LevelEditor::ResourcePanel::moveFileAcceptPayload(std::string const& virtual_path) {
		//Drop target
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(std::string(payload_typestring + "_FILE").c_str())) {
				//Get asset ID
				std::string asset_id(static_cast<const char*>(payload->Data));

				//Craft the destination path
				std::filesystem::path dest_path = NIKE_PATH_SERVICE->resolvePath(virtual_path) / asset_id;

				//Copy file
				std::filesystem::rename(NIKE_ASSETS_SERVICE->getAssetPath(asset_id), dest_path);

				//Update files
				files = NIKE_PATH_SERVICE->listFiles(current_path);
			}
			ImGui::EndDragDropTarget();
		}
	}

	void LevelEditor::ResourcePanel::onEvent(std::shared_ptr<Assets::FileDropEvent> event) {

		if (NIKE_LVLEDITOR_SERVICE->getEditorState() && !checkPopUpShowing()) {
			int file_count = event->count;
			const char** file_paths = event->paths;

			//Initialize message
			std::string message = "Files Added: " + std::to_string(file_count) + " \n";

			for (int i = 0; i < file_count; ++i) {
				std::filesystem::path src_file_path{ file_paths[i] };

				//Check if path is valid
				if (NIKE_ASSETS_SERVICE->isPathValid(src_file_path.string(), false)) {

					//Get asset id
					auto asset_id = NIKE_ASSETS_SERVICE->getIDFromPath(src_file_path.string(), false);

					//Check if asset has already been registered
					if (NIKE_ASSETS_SERVICE->isAssetRegistered(asset_id)) {
						//Delete assets old registration
						std::filesystem::remove(NIKE_ASSETS_SERVICE->getAssetPath(asset_id));
					}

					//Copy file
					std::filesystem::copy(src_file_path, NIKE_PATH_SERVICE->resolvePath(current_path), std::filesystem::copy_options::overwrite_existing);

					//Log success
					NIKEE_CORE_INFO("File " + src_file_path.string() + " successfully copied into" + NIKE_PATH_SERVICE->resolvePath(current_path).string());
					message += std::string(file_paths[i]) + "\n";
				}
				else {
					NIKEE_CORE_ERROR("Error Unsupported File Type: {}", file_paths[i]);
					message = "Error Unsupported File Type: " + src_file_path.filename().extension().string();
				}
			}

			//Update directories & files
			directories = NIKE_PATH_SERVICE->listDirectories(current_path);
			files = NIKE_PATH_SERVICE->listFiles(current_path);

			//Show success popup
			success_msg->assign(message);
			b_file_dropped = true;
		}

		event->setEventProcessed(true);
	}

	unsigned int LevelEditor::ResourcePanel::fileIcon(std::filesystem::path const& path) {
		//Get assets icons
		if (NIKE_ASSETS_SERVICE->getAssetType(path) == Assets::Types::Texture && NIKE_ASSETS_SERVICE->isAssetCached(path)) {

			//Check if asset has been loaded
			std::string icon_ref = NIKE_ASSETS_SERVICE->getIDFromPath(path.string(), false);
			return NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(icon_ref)->gl_data;
		}
		else {
			std::string icon_ref = path.extension().string().substr(1) + "_icon.png";
			if (auto texture = NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(icon_ref)) {
				return texture->gl_data;
			}
			else {
				//Load default file icon
				return NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>("def_icon.png")->gl_data;
			}
		}
	}

	void LevelEditor::ResourcePanel::renderAssetsBrowser(std::string const& virtual_path) {

		//Check if both files and directories are empty
		if (directories.empty() && files.empty()) {
			ImGui::Text("No results.");
			return;
		}

		//Calculate the number of icons per row based on window size & icon size
		int icons_per_row = static_cast<int>((ImGui::GetContentRegionAvail().x + (ImGui::GetStyle().ItemSpacing.x * 2)) / (icon_size.x + (ImGui::GetStyle().ItemSpacing.x * 2)));
		icons_per_row = std::clamp(icons_per_row, 1, (int)UINT16_MAX);

		//Track item index
		int itemIndex = 0;

		//Display all directories
		for (const auto& dir : directories) {

			//Skip dir not matching searching filter
			if (dir == NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/") || dir.string().find(search_filter) == dir.string().npos) {
				continue;
			}

			//Item to exist in the same row
			if (itemIndex % icons_per_row != 0) {
				ImGui::SameLine();
			}

			ImGui::BeginGroup();

			//Folder icon
			ImTextureID icon = static_cast<ImTextureID>(NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>("folder_icon.png")->gl_data);

			//Display directory icon
			ImVec2 uv0(0.0f, 1.0f);
			ImVec2 uv1(1.0f, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			if (ImGui::ImageButton(std::string("##" + dir.filename().string()).c_str(), icon, ImVec2(icon_size.x, icon_size.y), uv0, uv1)) {
				//Change current path to folder path clicked
				current_path = virtual_path + '/' + dir.filename().string();

				//Update directories & files
				directories = NIKE_PATH_SERVICE->listDirectories(current_path);
				files = NIKE_PATH_SERVICE->listFiles(current_path);

				//Break from files
				ImGui::PopStyleColor();
				ImGui::EndGroup();
				break;
			}
			ImGui::PopStyleColor();
			moveFileAcceptPayload(virtual_path + '/' + dir.filename().string());

			//Display directory name
			ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + icon_size.x);
			ImGui::TextWrapped(dir.filename().string().c_str());
			ImGui::PopTextWrapPos();

			ImGui::EndGroup();

			itemIndex++;
		}

		//Display all files
		for (const auto& file : files) {

			//Skip file not matching searching filter
			if (file.string().find(search_filter) == file.string().npos) {
				continue;
			}

			//Item to exist in the same row
			if (itemIndex % icons_per_row != 0) {
				ImGui::SameLine();
			}

			//Begin file group
			ImGui::BeginGroup();

			//Extension cases
			ImTextureID icon = static_cast<ImTextureID>(fileIcon(file));

			//Display file icon
			ImVec2 uv0(0.0f, 1.0f);
			ImVec2 uv1(1.0f, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			if (ImGui::ImageButton(std::string("##" + file.filename().string()).c_str(), icon, ImVec2(icon_size.x, icon_size.y), uv0, uv1)) {
				selected_asset_id = file.filename().string();
			}
			ImGui::PopStyleColor();

			//Start drag-and-drop source
			if (NIKE_ASSETS_SERVICE->isAssetRegistered(file.filename().string()) && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				auto filetype_string = NIKE_ASSETS_SERVICE->getAssetTypeString(file.filename().string());
				//Set drag payload with asset name
				ImGui::SetDragDropPayload(std::string(filetype_string + "_FILE").c_str(), file.filename().string().c_str(), file.filename().string().size() + 1);
				payload_typestring = filetype_string;

				//Render the icon or name at the cursor during dragging
				ImGui::Image(icon, { 64, 64 }, uv0, uv1);
				ImGui::TextWrapped(file.filename().string().c_str());
				ImGui::EndDragDropSource();
			}

			//Display file name
			ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + icon_size.x);
			ImGui::TextWrapped(file.filename().string().c_str());
			ImGui::PopTextWrapPos();

			ImGui::EndGroup();

			itemIndex++;
		}
	}

	int LevelEditor::ResourcePanel::TextCallback(ImGuiInputTextCallbackData* data) {
		EditorState* editor_state = static_cast<EditorState*>(data->UserData);
		editor_state->cursor_pos = data->CursorPos;
		return 0;
	}

	void LevelEditor::ResourcePanel::extractCurrentWord(std::string const& content, size_t cursor_pos, std::string& buffer) {
		size_t word_start = content.find_last_of(" \n\t", cursor_pos - 1);
		word_start = (word_start == std::string::npos) ? 0 : word_start + 1;
		size_t word_end = content.find_first_of(" \n\t", cursor_pos);
		word_end = (word_end == std::string::npos) ? content.size() : word_end;
		buffer = content.substr(word_start, word_end - word_start);
	}

	void LevelEditor::ResourcePanel::showLuaIntellisense(std::string& content, size_t cursor_pos, std::string& buffer) {

		//Search for matching functions
		if (!buffer.empty()) {
			for (const auto& func : NIKE_LUA_SERVICE->getGlobalLuaFunctions()) {
				if (func.find(buffer.c_str()) != std::string::npos) {
					if (ImGui::Selectable(func.c_str())) {
						auto pos = cursor_pos - buffer.size();
						content.erase(pos, buffer.size());
						content.insert(pos, func.c_str());
					}
				}
			}
		}
	}

	void LevelEditor::ResourcePanel::renderFileEditor() {
		for (decltype(file_editing_map)::iterator it = file_editing_map.begin(); it != file_editing_map.end(); ++it) {
			ImGui::Begin(it->first.c_str());

			//Editing file
			static EditorState editor_state;
			ImGui::Text("Editing: %s", it->first.c_str());
			if (ImGui::InputTextMultiline("##editor", &it->second[0], it->second.capacity(),
				ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
				ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackAlways, TextCallback, &editor_state)) {
				it->second.resize(strlen(it->second.c_str()));
			}

			//Special lua intellisense
			if (NIKE_ASSETS_SERVICE->getAssetType(it->first) == Assets::Types::Script) {
				//Lua intellisense
				static std::string current_word;
				current_word.reserve(64);
				extractCurrentWord(it->second, editor_state.cursor_pos, current_word);
				showLuaIntellisense(it->second, editor_state.cursor_pos, current_word);
			}

			//Save file
			if (ImGui::Button("Save##Save file") || (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))) {
				std::ofstream file(NIKE_ASSETS_SERVICE->getAssetPath(it->first));
				if (file.is_open()) {
					file << it->second;
					file.close();
					NIKEE_CORE_INFO("File saved.");
				}
				else {
					NIKEE_CORE_WARN("Failed to save file");
					file.close();
				}
			}

			ImGui::SameLine();

			//Close file
			if (ImGui::Button("Close##CloseFile")) {
				it = file_editing_map.erase(it);
				ImGui::End();
				break;
			}

			ImGui::End();
		}
	}

	std::function<void()> LevelEditor::ResourcePanel::deleteAssetPopup(std::string const& popup_id) {
		return [this, popup_id]() {

			//Warning message
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "This action cannot be undone!");

			//Select a component to add
			ImGui::Text("Are you sure you want to delete this asset?");

			//Add spacing
			ImGui::Spacing();

			//Display each component as a button
			if (ImGui::Button("Confirm")) {

				//Get selected asset path
				auto path = NIKE_ASSETS_SERVICE->getAssetPath(selected_asset_id);

				//Remove path and clear selected asset text buffer
				std::filesystem::remove(path);
				selected_asset_id.clear();
				files = NIKE_PATH_SERVICE->listFiles(current_path);

				//Close popup
				closePopUp(popup_id);
			}

			//Same line
			ImGui::SameLine();

			//Cancel deleting asset
			if (ImGui::Button("Cancel")) {

				//Close popup
				closePopUp(popup_id);
			}
		};
	}

	std::function<void()> LevelEditor::ResourcePanel::deleteDirectoryPopup(std::string const& popup_id) {
		return [this, popup_id]() {

			//Warning message
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "This action cannot be undone!");

			//Select a component to add
			ImGui::Text("Are you sure you want to delete everything in selected directory?");

			//Add spacing
			ImGui::Spacing();

			//Display each component as a button
			if (ImGui::Button("Confirm")) {

				//Check for directory mode
				switch (directory_mode) {
				case 0: {

					//Remove all files in current directory
					for (auto const& file : files) {
						std::filesystem::remove(file);
					}
					break;
				}
				case 1: {

					//Remove all files & folders in current directory
					for (auto const& file : files) {
						std::filesystem::remove(file);
					}
					for (auto const& dir : directories) {
						std::filesystem::remove_all(dir);
					}
					break;
				}
				case 2: {

					//Remove all files & folders in root directory
					for (auto const& file : NIKE_PATH_SERVICE->listFiles(root_path)) {
						std::filesystem::remove(file);
					}
					for (auto const& dir : NIKE_PATH_SERVICE->listDirectories(root_path)) {
						std::filesystem::remove_all(dir);
					}
					current_path = root_path;
					break;
				}
				default: {
					break;
				}
				}


				//Update directories & files
				directories = NIKE_PATH_SERVICE->listDirectories(current_path);
				files = NIKE_PATH_SERVICE->listFiles(current_path);

				//Close popup
				closePopUp(popup_id);
			}

			//Same line
			ImGui::SameLine();

			//Cancel deleting asset
			if (ImGui::Button("Cancel")) {

				//Close popup
				closePopUp(popup_id);
			}
			};
	}

	std::function<void()> LevelEditor::ResourcePanel::newFolderPopup(std::string const& popup_id) {
		return [this, popup_id]() {

			//Select a component to add
			ImGui::Text("New folder name: ");

			//New folder name
			static std::string folder_name = "";
			folder_name.resize(32);
			ImGui::InputText("##NewFolderName", folder_name.data(), folder_name.capacity() + 1);

			//Add spacing
			ImGui::Spacing();

			//Display each component as a button
			if (ImGui::Button("Create")) {

				//Create a new directory
				std::filesystem::create_directory(NIKE_PATH_SERVICE->resolvePath(current_path) / folder_name);

				//Update directories & files
				directories = NIKE_PATH_SERVICE->listDirectories(current_path);

				//Reset folder name buffer
				folder_name.assign("");

				//Close popup
				closePopUp(popup_id);
			}

			//Same line
			ImGui::SameLine();

			//Cancel deleting asset
			if (ImGui::Button("Cancel")) {

				//Reset folder name buffer
				folder_name.assign("");

				//Close popup
				closePopUp(popup_id);
			}
			};
	}

	void LevelEditor::ResourcePanel::init() {

		//Setup events listening
		std::shared_ptr<LevelEditor::ResourcePanel> resourcepanel_wrapped(this, [](LevelEditor::ResourcePanel*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Assets::FileDropEvent>(resourcepanel_wrapped);

		entities_panel = std::dynamic_pointer_cast<EntitiesPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(EntitiesPanel::getStaticName()));

		//Register popups
		error_msg = std::make_shared<std::string>("Error");
		success_msg = std::make_shared<std::string>("Success");
		registerPopUp("Error", defPopUp("Error", error_msg));
		registerPopUp("Success", defPopUp("Success", success_msg));
		registerPopUp("Delete Asset", deleteAssetPopup("Delete Asset"));
		registerPopUp("Clear Directory", deleteDirectoryPopup("Clear Directory"));
		registerPopUp("New Folder", newFolderPopup("New Folder"));

		//Initialize root
		root_path = "Game_Assets:/";
		current_path = root_path;

		//Search up till 32 characters
		search_filter.resize(32);
		search_filter = "";

		//Default icon size
		icon_size = { 128.0f, 128.0f };

		//Register all engine icons
		NIKE_ASSETS_SERVICE->scanAssetDirectory("Engine_Assets:/Icons");

		//Init all directories & files
		directories = NIKE_PATH_SERVICE->listDirectories(current_path);
		files = NIKE_PATH_SERVICE->listFiles(current_path);

		//Setup directory watching 
		NIKE_PATH_SERVICE->watchDirectoryTree("Game_Assets:/", [this](std::filesystem::path const& file, filewatch::Event event) {

			//Enngine engine assets path
			static auto engine_assets = NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/");

			//Skip directories
			if (std::filesystem::is_directory(file) ||
				!NIKE_ASSETS_SERVICE->isPathValid(file.string(), false) ||
				file == engine_assets) {
				return;
			}

			//Watch for events
			switch (event) {
			case filewatch::Event::added: {

				//Push to file event queue
				file_event_queue.push([file]() {

					//Register asset if needed
					auto asset_id = NIKE_ASSETS_SERVICE->getIDFromPath(file.string(), false);
					if (!NIKE_ASSETS_SERVICE->isAssetRegistered(asset_id)) {
						NIKE_ASSETS_SERVICE->registerAsset(file.string(), false);
					}
					});
				break;
			}
			case filewatch::Event::removed: {

				//Push to file event queue
				file_event_queue.push([file]() {
					//Unregister asset if needed
					NIKE_ASSETS_SERVICE->unregisterAsset(NIKE_ASSETS_SERVICE->getIDFromPath(file.string(), false));
					});
				break;
			}
			case filewatch::Event::modified: {

				//Push to file event queue
				file_event_queue.push([file]() {

					//Only recache assets that are already cached
					auto asset_id = NIKE_ASSETS_SERVICE->getIDFromPath(file.string(), false);
					if (NIKE_ASSETS_SERVICE->isAssetCached(asset_id)) {

						//Recache asset
						NIKE_ASSETS_SERVICE->recacheAsset(asset_id);
					}
					});

				break;
			}
			default: {
				break;
			}
			}
			});
	}

	void LevelEditor::ResourcePanel::update() {
		//Update resource panel with file change events
		while (!file_event_queue.empty()) {
			//Execute event
			try
			{
				auto& func = file_event_queue.front();
				// Check if the callable is null
				if (!func)  
				{
					
					
				}
				func();
			}
			catch (const std::exception&)
			{
				NIKEE_CORE_ERROR("**THIS** nullptr");
			}

			//Pop from queue
			file_event_queue.pop();
		}
	}

	void LevelEditor::ResourcePanel::render() {
		if (ImGui::Begin(getName().c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {

			ImGui::BeginMenuBar();

			//Parent path navigation
			{
				//Back button
				if (!current_path.empty() && ImGui::Button("< Back")) {

					//Stop searching for parent at root directory
					if (current_path != root_path) {
						current_path = NIKE_PATH_SERVICE->getVirtualParentPath(current_path);

						//Update directories & files
						directories = NIKE_PATH_SERVICE->listDirectories(current_path);
						files = NIKE_PATH_SERVICE->listFiles(current_path);
					}
				}
				moveFileAcceptPayload(NIKE_PATH_SERVICE->getVirtualParentPath(current_path));
			}

			ImGui::Spacing();

			//New folder
			{
				//Create new folder popup
				if (ImGui::Button("New Folder")) {
					openPopUp("New Folder");
				}
			}

			ImGui::Spacing();

			//Directory level actions
			{
				//Array of load directories
				const char* load_directory[] = { "Current", "Current *", "Root *" };

				//Render the dropdown
				ImGui::PushItemWidth(100.0f);
				ImGui::Combo("##Directory", &directory_mode, load_directory, IM_ARRAYSIZE(load_directory));
				ImGui::PopItemWidth();

				//Load all from directory
				if (ImGui::Button("Load All")) {

					//Check for directory mode
					switch (directory_mode) {
					case 0: {
						NIKE_ASSETS_SERVICE->cacheAssetDirectory(current_path);
						success_msg->assign("All assets in: \"" + current_path + "\" loaded.");
						openPopUp("Success");
						break;
					}
					case 1: {
						NIKE_ASSETS_SERVICE->cacheAssetDirectory(current_path, true);
						success_msg->assign("All assets in: \"" + current_path + "*\" loaded.");
						openPopUp("Success");
						break;
					}
					case 2: {
						NIKE_ASSETS_SERVICE->cacheAssetDirectory(root_path, true);
						success_msg->assign("All assets in: \"" + root_path + "*\" loaded.");
						openPopUp("Success");
						break;
					}
					default: {
						break;
					}
					}
				}

				//Unload all from directory
				if (ImGui::Button("Unload All")) {

					//Check for directory mode
					switch (directory_mode) {
					case 0: {
						NIKE_ASSETS_SERVICE->uncacheAssetDirectory(current_path);
						success_msg->assign("All assets in: \"" + current_path + "*\" unloaded.");
						openPopUp("Success");
						break;
					}
					case 1: {
						NIKE_ASSETS_SERVICE->uncacheAssetDirectory(current_path, true);
						success_msg->assign("All assets in: \"" + current_path + "*\" unloaded.");
						openPopUp("Success");
						break;
					}
					case 2: {
						NIKE_ASSETS_SERVICE->uncacheAssetDirectory(root_path, true);
						success_msg->assign("All assets in: \"" + root_path + "*\" unloaded.");
						openPopUp("Success");
						break;
					}
					default: {
						break;
					}
					}
				}

				//Delete all from directory
				if (ImGui::Button("Delete All")) {
					openPopUp("Clear Directory");
				}
			}

			ImGui::Spacing();

			//Customize icon size
			{
				ImGui::Text("Icon Size: ");
				ImGui::PushItemWidth(50.0f);
				ImGui::DragFloat("##IconSizing", &icon_size.x, 1.0f, 32.0f, 256.0f, "%.f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::PopItemWidth();
				icon_size.y = icon_size.x;
			}

			ImGui::Spacing();

			//Search filter
			{
				//Input filter
				ImGui::Text("Filter: ");
				ImGui::SameLine();
				ImGui::PushItemWidth(100.0f);
				if (ImGui::InputTextWithHint("##SearchFilter", "Search...", search_filter.data(), search_filter.capacity() + 1)) {
					search_filter.resize(strlen(search_filter.c_str()));
				}
				ImGui::PopItemWidth();
			}

			ImGui::Spacing();

			//Refresh directory
			{
				if (ImGui::Button("Refresh")) {
					//Update directories & files
					directories = NIKE_PATH_SERVICE->listDirectories(current_path);
					files = NIKE_PATH_SERVICE->listFiles(current_path);
				}
			}

			//Render popups
			renderPopUps();

			ImGui::EndMenuBar();

			//Render all assets & folders
			renderAssetsBrowser(current_path);
		}

		//Render selected asset options
		if (!selected_asset_id.empty() && NIKE_ASSETS_SERVICE->isAssetRegistered(selected_asset_id)) {

			// Center the panel
			ImGui::Begin("Selected Asset", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings);

			//Get selected asset path
			auto path = NIKE_ASSETS_SERVICE->getAssetPath(selected_asset_id);

			//Asset metadata
			ImGui::Text("Asset: %s", selected_asset_id.c_str());
			ImGui::Text("Type: %s", NIKE_ASSETS_SERVICE->getAssetTypeString(selected_asset_id).c_str());

			//Get selected asset texture display
			ImTextureID display = static_cast<ImTextureID>(fileIcon(path));

			//Display image
			ImVec2 uv0(0.0f, 1.0f);
			ImVec2 uv1(1.0f, 0.0f);
			ImGui::Image(display, { 256, 256 }, uv0, uv1);

			//Loadable type actions
			if (NIKE_ASSETS_SERVICE->isAssetLoadable(selected_asset_id)) {
				//Asset loading or unloading
				if (NIKE_ASSETS_SERVICE->isAssetCached(selected_asset_id)) {
					//Unload action
					if (ImGui::Button("Unload")) {

						//Unload asset
						NIKE_ASSETS_SERVICE->uncacheAsset(selected_asset_id);
						success_msg->assign("Asset: \"" + selected_asset_id + "\" unloaded.");
						openPopUp("Success");
					}

					//Audio asset preview
					if (NIKE_ASSETS_SERVICE->getAssetType(selected_asset_id) == Assets::Types::Sound ||
						NIKE_ASSETS_SERVICE->getAssetType(selected_asset_id) == Assets::Types::Music) {

						//Same line
						ImGui::SameLine();

						//Play button
						if (ImGui::Button("Play")) {
							//Check if channel group has been created
							if (!NIKE_AUDIO_SERVICE->checkChannelGroupExist("Audio Preview")) {
								NIKE_AUDIO_SERVICE->createChannelGroup("Audio Preview");
							}

							//Get audio group
							auto group = NIKE_AUDIO_SERVICE->getChannelGroup("Audio Preview");

							//Toggle audio state
							if (group->getPaused()) {
								group->setPaused(false);
							}
							else {
								//Play music
								bool is_music = NIKE_ASSETS_SERVICE->getAssetType(selected_asset_id) == Assets::Types::Music ? true : false;
								NIKE_AUDIO_SERVICE->playAudio(selected_asset_id, "", "Audio Preview", 0.5f, 0.5f, false, is_music);
							}
						}

						//Manage preview audio group
						if (NIKE_AUDIO_SERVICE->checkChannelGroupExist("Audio Preview")) {
							auto group = NIKE_AUDIO_SERVICE->getChannelGroup("Audio Preview");

							if (group->isPlaying()) {
								//Same line
								ImGui::SameLine();

								//Pause audio preview
								if (ImGui::Button("Pause")) {
									group->setPaused(true);
								}

								//Same line
								ImGui::SameLine();

								//Pause audio preview
								if (ImGui::Button("Stop")) {
									group->stop();
								}
							}
							else if (!group->isPlaying() && !group->getPaused()) {
								NIKE_AUDIO_SERVICE->unloadChannelGroup("Audio Preview");
							}
						}
					}


				}
				else {
					//Load action
					if (ImGui::Button("Load")) {

						//Load asset
						NIKE_ASSETS_SERVICE->cacheAsset(selected_asset_id);
						success_msg->assign("Asset: \"" + selected_asset_id + "\" loaded.");
						openPopUp("Success");
					}
				}

				//Same line
				ImGui::SameLine();
			}

			//Editable type actions
			if (NIKE_ASSETS_SERVICE->isAssetEditable(selected_asset_id)) {
				if (ImGui::Button("Edit##EditableAsset")) {

					//Read file into string
					std::ifstream file(NIKE_ASSETS_SERVICE->getAssetPath(selected_asset_id));
					if (file.is_open()) {

						//Check if file is already open
						if (file_editing_map.find(selected_asset_id) == file_editing_map.end()) {
							file_editing_map[selected_asset_id].reserve(1024 * 1024); // 1mb storage for file editing
							// Read file content
							file_editing_map[selected_asset_id].assign((std::istreambuf_iterator<char>(file)),
								std::istreambuf_iterator<char>());
							file.close();
						}
					}
					else {
						NIKEE_CORE_WARN("Failed to open file");
						file.close();
					}
				}

				//Same line
				ImGui::SameLine();
			}

			//Delete asset
			if (ImGui::Button("Delete##DeleteAsset")) {
				openPopUp("Delete Asset");
			}

			//Same line
			ImGui::SameLine();

			//Unload action
			if (ImGui::Button("Close##CloseAsset")) {

				//Reset selected asset id
				selected_asset_id.clear();
			}

			//Render popups
			renderPopUps();

			ImGui::End();
		}

		//Render file editor
		renderFileEditor();

		//File dropped popup
		if (b_file_dropped && !checkPopUpShowing()) {
			openPopUp("Success");
			b_file_dropped = false;
		}

		//Render popups
		renderPopUps();

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
		if (ImGui::Combo("##CameraSelector", &combo_index, cam_name, &cam_entities, static_cast<int>(cam_entities.size()))) {
			dispatch = true;
		}

		// Ensure combo_index is valid
		if (combo_index < 0 || combo_index >= cam_entities.size()) {
			combo_index = 0;

		}

		// Ensure last_dispatched_index is valid
		if (last_dispatched_index < 0 || last_dispatched_index >= cam_entities.size()) {
			last_dispatched_index = 0;

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

		// Used to check if mouse is over the viewport
		ImGuiIO& io = ImGui::GetIO();
		auto game_window = std::dynamic_pointer_cast<GameWindowPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(GameWindowPanel::getStaticName()));


		//If free camera is active
		if (it->first == UINT16_MAX) {
			// Position Controls
			ImGui::Text("Position:");

			if (ImGui::Button("Up") || ImGui::IsItemActive() || !checkPopUpShowing() && game_window->isMouseInWindow() && NIKE_INPUT_SERVICE->isKeyPressed(NIKE_KEY_UP)) {
				// Move camera position up
				active_cam.position.y += 500.0f * ImGui::GetIO().DeltaTime;
			}

			cameraChangeAction(active_cam, cam_before_change);

			ImGui::SameLine();

			if (ImGui::Button("Down") || ImGui::IsItemActive() || !checkPopUpShowing() && game_window->isMouseInWindow() && NIKE_INPUT_SERVICE->isKeyPressed(NIKE_KEY_DOWN)) {
				// Move camera position down
				active_cam.position.y -= 500.0f * ImGui::GetIO().DeltaTime;
			}

			cameraChangeAction(active_cam, cam_before_change);

			ImGui::SameLine();

			if (ImGui::Button("Left") || ImGui::IsItemActive() || !checkPopUpShowing() && game_window->isMouseInWindow() && NIKE_INPUT_SERVICE->isKeyPressed(NIKE_KEY_LEFT)) {
				// Move camera position left
				active_cam.position.x -= 500.0f * ImGui::GetIO().DeltaTime;
			}

			cameraChangeAction(active_cam, cam_before_change);

			ImGui::SameLine();

			if (ImGui::Button("Right") || ImGui::IsItemActive() || !checkPopUpShowing() && game_window->isMouseInWindow() && NIKE_INPUT_SERVICE->isKeyPressed(NIKE_KEY_RIGHT)) {
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

		// Scroll To Zoom
		if (!checkPopUpShowing() && game_window->isMouseInWindow()) {
			active_cam.zoom -= io.MouseWheel * ImGui::GetIO().DeltaTime;
			active_cam.zoom = std::clamp(active_cam.zoom, EPSILON, (float)UINT16_MAX);
		}

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
	* User Interface Management Panel
	*********************************************************************/
	std::function<void()> LevelEditor::UIPanel::createButtonPopup(std::string const& popup_id) {
		return [this, popup_id]() {

			//Button ID
			static std::string btn_id = "";
			btn_id.reserve(64);
			{
				//Enter new button ID
				ImGui::Text("New button id: ");

				//New Btn ID
				if (ImGui::InputText("##BtnID", btn_id.data(), btn_id.capacity() + 1)) {
					btn_id.resize(strlen(btn_id.c_str()));
				}
			}

			ImGui::Separator();

			//Button Text
			static Render::Text btn_text;
			btn_text.text.reserve(64);
			static int font_index = -1;
			{
				// Get all loaded fonts
				const auto& all_loaded_fonts = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Font);

				// Display combo box for font selection
				ImGui::Text("Button Font: ");
				if (ImGui::Combo("##SelectBtnFont", &font_index, all_loaded_fonts.data(), static_cast<int>(all_loaded_fonts.size()))) {
					// Validate the selected index and get the new font ID
					if (font_index >= 0 && font_index < static_cast<int>(all_loaded_fonts.size())) {
						btn_text.font_id = all_loaded_fonts[font_index];
					}
				}

				//Enter new button text
				ImGui::Text("Button Text: ");
				if (ImGui::InputText("##btn_text", btn_text.text.data(), btn_text.text.capacity() + 1)) {
					btn_text.text.resize(strlen(btn_text.text.c_str()));
				}

				//Adjust btn color
				ImGui::Text("Button Text Color: ");
				ImGui::ColorEdit4("##BtnTextColor", &btn_text.color.x, ImGuiColorEditFlags_AlphaBar);
			}

			ImGui::Separator();

			//Static btn transform
			static Transform::Transform btn_transform;
			{
				//Edit Position
				{
					//Drag position
					ImGui::Text("Position");
					ImGui::DragFloat2("##BtnTransform", &btn_transform.position.x, 0.1f);
				}

				//Edit Scale
				{
					//Change scale
					ImGui::Text("Scale");
					ImGui::DragFloat2("##BtnScale", &btn_transform.scale.x, 0.1f, 0.f, (float)UINT16_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
				}

				////Edit Rotation ( Temporarily Disabled )
				//{
				//	//Change rotation
				//	ImGui::Text("Rotation");
				//	ImGui::DragFloat("##BtnRotation", &btn_transform.rotation, 0.1f, -360.f, 360.f);
				//}
			}

			ImGui::Separator();

			static int texture_index = -1;
			static int model_index = -1;
			static std::string render_ref = "";
			static bool b_model = true;
			static Vector4f btn_color = { 1.0f, 1.0f, 1.0f, 1.0f };
			//Select texture or model
			{
				//Select render mode
				ImGui::Text("Button Render: ");
				ImGui::SameLine();
				if (ImGui::SmallButton(b_model ? "Model" : "Texture")) {
					b_model = !b_model;
				}

				//Select ref based on mode
				if (!b_model) {
					// Get all loaded fonts
					const auto& all_loaded_texture = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Texture);

					// Display combo box for font selection
					ImGui::Text("Button Texture: ");
					if (ImGui::Combo("##SelectBtnTexture", &texture_index, all_loaded_texture.data(), static_cast<int>(all_loaded_texture.size()))) {
						// Validate the selected index and get the new font ID
						if (texture_index >= 0 && texture_index < static_cast<int>(all_loaded_texture.size())) {
							render_ref = all_loaded_texture[texture_index];
						}
					}
				}
				else {
					// Get all loaded fonts
					const auto& all_loaded_models = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Model);

					// Display combo box for font selection
					ImGui::Text("Button Shape: ");
					if (ImGui::Combo("##SelectBtnShape", &model_index, all_loaded_models.data(), static_cast<int>(all_loaded_models.size()))) {
						// Validate the selected index and get the new font ID
						if (model_index >= 0 && model_index < static_cast<int>(all_loaded_models.size())) {
							render_ref = all_loaded_models[model_index];
						}
					}

					//Adjust btn color
					ImGui::Text("Button Color: ");
					ImGui::ColorEdit4("##BtnColor", &btn_color.x, ImGuiColorEditFlags_AlphaBar);
				}
			}

			//Add spacing
			ImGui::Spacing();

			//Display each component as a button
			if (ImGui::Button("Create") && !btn_id.empty() && !NIKE_UI_SERVICE->checkUIEntity(btn_id)) {

				//Make copies
				Transform::Transform trans_copy = btn_transform;
				Render::Text txt_copy = btn_text;

				//Create button based on mode
				if (b_model) {
					//Create button
					auto entity = NIKE_UI_SERVICE->createButton(btn_id, std::move(trans_copy), std::move(txt_copy), Render::Shape(render_ref, btn_color));
					auto const& e_panel = entities_panel.lock();
					EntityMetaData copy = e_panel->getEntityMetaData(entity);
					copy.entity_id = btn_id;
					e_panel->setEntityMetaData(entity, copy);
				}
				else {
					//Create button
					auto entity = NIKE_UI_SERVICE->createButton(btn_id, std::move(trans_copy), std::move(txt_copy), Render::Texture(render_ref, btn_color, false, 0.5f, true));
					auto const& e_panel = entities_panel.lock();
					EntityMetaData copy = e_panel->getEntityMetaData(entity);
					copy.entity_id = btn_id;
					e_panel->setEntityMetaData(entity, copy);
				}

				//Reset btn id
				btn_id.assign("");

				//Reset btn transform
				btn_transform = Transform::Transform();

				//Reset btn text
				btn_text = Render::Text();

				//Reset font current index
				font_index = -1;

				//Reset render modes
				texture_index = -1;
				model_index = -1;
				render_ref = "";
				b_model = true;
				btn_color = { 1.0f, 1.0f, 1.0f, 1.0f };

				//Close popup
				closePopUp(popup_id);
			}

			//Same line
			ImGui::SameLine();

			//Cancel createing new btn
			if (ImGui::Button("Cancel")) {

				//Reset btn id
				btn_id.assign("");

				//Reset btn transform
				btn_transform = Transform::Transform();

				//Reset btn text
				btn_text = Render::Text();

				//Reset font current index
				font_index = -1;

				//Reset render modes
				texture_index = -1;
				model_index = -1;
				render_ref = "";
				b_model = true;
				btn_color = { 1.0f, 1.0f, 1.0f, 1.0f };

				//Close popup
				closePopUp(popup_id);
			}
			};
	}

	void LevelEditor::UIPanel::init() {
		registerPopUp("Create Button", createButtonPopup("Create Button"));

		entities_panel = std::dynamic_pointer_cast<EntitiesPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(EntitiesPanel::getStaticName()));
	}

	void LevelEditor::UIPanel::update() {

	}

	void LevelEditor::UIPanel::render() {
		ImGui::Begin(getName().c_str());

		//Create button
		{
			if (ImGui::Button("Create Button")) {
				openPopUp("Create Button");
			}
		}

		ImGui::Separator();

		//List of buttons
		auto& buttons = NIKE_UI_SERVICE->getAllButtons();
		if (buttons.empty()) {
			ImGui::Text("No active buttons.");
		}
		else {
			ImGui::Text("Active Buttons: ");
		}

		//Show all active buttons
		for (auto& button : buttons) {
			ImGui::Spacing();

			//Collapsing button
			if (ImGui::CollapsingHeader(std::string("Button: " + button.first).c_str(), ImGuiTreeNodeFlags_None)) {

				//Set trigger state
				{
					//Select Input State
					static int trigger_index = 0;
					trigger_index = static_cast<int>(button.second.input_state);
					ImGui::Text("Button Input State: ");
					if (ImGui::Combo("##BtnInputState", &trigger_index, "Pressed\0Triggered\0Released\0")) {
						button.second.input_state = static_cast<UI::InputStates>(trigger_index);
					}
				}

				//Select script
				{
					// Get all loaded scripts
					const auto& get_load_scripts = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Script);

					// Find the index of the currently selected script id in the list
					int script_index = -1;
					for (size_t i = 0; i < get_load_scripts.size(); ++i) {
						if (button.second.script.script_id == get_load_scripts[i]) {
							script_index = static_cast<int>(i);
							break;
						}
					}

					//Display combo box for script selection
					ImGui::Text("Button Script: ");
					if (ImGui::Combo("##BtnScript", &script_index, get_load_scripts.data(), static_cast<int>(get_load_scripts.size()))) {
						// Validate the selected index and get the new font ID
						if (script_index >= 0 && script_index < static_cast<int>(get_load_scripts.size())) {
							button.second.script.script_id = get_load_scripts[script_index];
						}
					}
				}

				//Select script function
				{
					if (!button.second.script.script_id.empty() && NIKE_ASSETS_SERVICE->isAssetRegistered(button.second.script.script_id)) {
						// Get all loaded script functions
						const auto& get_script_functions = NIKE_LUA_SERVICE->getScriptFunctions(NIKE_ASSETS_SERVICE->getAssetPath(button.second.script.script_id));
						std::vector<const char*> funcs;
						//Convert to const char*
						std::for_each(get_script_functions.begin(), get_script_functions.end(), [&funcs](std::string const& ref) {
							funcs.push_back(ref.c_str());
							});

						// Find the index of the currently selected script id in the list
						int script_func_index = -1;
						for (size_t i = 0; i < funcs.size(); ++i) {
							if (button.second.script.function == funcs[i]) {
								script_func_index = static_cast<int>(i);
								break;
							}
						}

						//Display combo box for script function selection
						ImGui::Text("Button Script Function: ");
						if (ImGui::Combo("##BtnScriptFunc", &script_func_index, funcs.data(), static_cast<int>(funcs.size()))) {
							// Validate the selected index and get the new font ID
							if (script_func_index >= 0 && script_func_index < static_cast<int>(funcs.size())) {
								button.second.script.function = funcs[script_func_index];
							}
						}
					}
				}

				//Set variadic arguements
				{
					//Get arguments
					auto& args = button.second.script.named_args;

					//Display all arguments
					ImGui::Text("Arguments:");
					if (args.empty()) {
						ImGui::Text("No arguments.");
					}
					for (auto it = args.begin(); it != args.end(); ++it) {
						const auto& key = it->first;
						const auto& value = it->second;

						//Display arguments
						if (ImGui::CollapsingHeader(std::string("Key: " + key).c_str(), ImGuiTreeNodeFlags_Bullet)) {

							//Display value based on its type
							std::visit(
								[&](auto&& arg) {
									using T = std::decay_t<decltype(arg)>;
									if constexpr (std::is_same_v<T, int>) {
										ImGui::Text("Value (int): %d", arg);
									}
									else if constexpr (std::is_same_v<T, float>) {
										ImGui::Text("Value (float): %.2f", arg);
									}
									else if constexpr (std::is_same_v<T, std::string>) {
										ImGui::Text("Value (string): %s", arg.c_str());
									}
									else if constexpr (std::is_same_v<T, bool>) {
										ImGui::Text("Value (bool): %s", arg ? "true" : "false");
									}
								},
								value
							);

							//Remove argument button
							if (ImGui::Button("Remove Argument")) {
								it = args.erase(it);
								break;
							}
						}
					}

					//Seperator
					ImGui::Separator();

					//Static variables
					static int val_type_index = 0;
					static std::string named_key = "";
					named_key.reserve(64);
					static std::string str_val = "";
					str_val.reserve(64);
					static int int_val = 0;
					static float float_val = 0.0f;
					static bool bool_val = false;

					//Set arguments
					if (ImGui::InputText("Input Key", named_key.data(), named_key.capacity() + 1)) {
						named_key.resize(strlen(named_key.c_str()));
					}

					//Select value type
					ImGui::Combo("Value Type", &val_type_index, "Int\0Float\0String\0Bool\0");

					//Take in input based on selected combo
					switch (val_type_index) {
					case 0: {
						ImGui::InputInt("Value (int)", &int_val);
						break;
					}
					case 1: {
						ImGui::InputFloat("Value (float)", &float_val);
						break;
					}
					case 2: {
						if (ImGui::InputText("Value (string)", str_val.data(), str_val.capacity() + 1)) {
							str_val.resize(strlen(str_val.c_str()));
						}
						break;
					}
					case 3: {
						ImGui::Checkbox("Value (bool)", &bool_val);
						break;
					}
					default: {
						break;
					}
					}

					//Add argument button
					if (ImGui::Button("Add Argument")) {

						//Check if named key is correct
						if (!named_key.empty() && args.find(named_key) == args.end()) {
							switch (val_type_index) {
							case 0: {
								args[named_key] = int_val;
								break;
							}
							case 1: {
								args[named_key] = float_val;
								break;
							}
							case 2: {
								args[named_key] = str_val;
								break;
							}
							case 3: {
								args[named_key] = bool_val;
								break;
							}
							default: {
								break;
							}
							}
						}
					}
				}

				//Add spacing
				ImGui::Spacing();

				//Delete Button
				if (ImGui::Button("Delete Button")) {
					NIKE_UI_SERVICE->destroyButton(button.first);
					break;
				}
			}

		}

		renderPopUps();

		ImGui::End();
	}

	/*****************************************************************//**
	* Tile Map Management Panel
	*********************************************************************/
	void LevelEditor::TileMapPanel::init() {
		entities_panel = std::dynamic_pointer_cast<EntitiesPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(EntitiesPanel::getStaticName()));
		registerPopUp("Save Grid", saveGridPopUp("Save Grid"));
		error_msg = std::make_shared<std::string>("Saving Error, Please load a scene first before saving!");
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
			//Adjust snap to grid state
			ImGui::Text("Snap entity to grid: ");
			ImGui::SameLine();
			ImGui::SmallButton(b_snap_to_grid ? "Enable##Snapping" : "Disable##Snapping");

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
				grid_size = NIKE_MAP_SERVICE->getGridSize();
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

			//Update grid size
			if (!ImGui::IsItemActive()) {
				grid_size = NIKE_MAP_SERVICE->getGridSize();
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
			ImGui::DragFloat2("##Cell Size", &cell_size.x, 0.1f, 0, INT16_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);

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

			//Update cell size
			if (!ImGui::IsItemActive()) {
				cell_size = NIKE_MAP_SERVICE->getCellSize();
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
			ImGui::DragFloat("##Grid Thickness", &grid_thickness, 0.1f, 1.0f, 10.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

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

		// Init cell
		Map::Cell target{ 1,2 }; Map::Cell start{ 5,5 };
		auto path = NIKE_MAP_SERVICE->findPath(start, target);
		NIKE_MAP_SERVICE->PrintPath(path);

		renderPopUps();

		ImGui::End();
	}

	void LevelEditor::TileMapPanel::saveGrid(std::filesystem::path scn_id)
	{
		std::string grid_file_name = Utility::extractFileName(scn_id.string());

		std::filesystem::path path = NIKE_PATH_SERVICE->resolvePath("Game_Assets:/Grids");

		// Serialize the grid data using the grid service
		nlohmann::json grid_data = NIKE_MAP_SERVICE->serialize();

		// Saving of grid
		if (!grid_file_name.empty() && (grid_file_name.find(".grid") == grid_file_name.npos) && !NIKE_ASSETS_SERVICE->isAssetRegistered(grid_file_name)) {

			//Craft file path from name
			if (std::filesystem::exists(path)) {
				path /= std::string(grid_file_name + ".grid");
			}
			else {
				path = NIKE_PATH_SERVICE->resolvePath("Game_Assets:/");
				path /= std::string(grid_file_name + ".grid");
			}
		}

		// Open the file for writing
		std::ofstream file(path.string(), std::ios::out | std::ios::trunc);

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

	void LevelEditor::TileMapPanel::removeGrid(std::filesystem::path scn_id)
	{
		std::string grid_file_name = Utility::extractFileName(scn_id.string()) + ".grid";

		std::string grid_path = NIKE_ASSETS_SERVICE->getAssetPath(grid_file_name).string();

		// Saving of grid
		if (std::filesystem::exists(grid_path)) {
			// Attempt to remove the grid file
			std::filesystem::remove(grid_path);
		}
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

				//// For saving of the prefab file with the extension
				//std::string grid_full_path = NIKE_ASSETS_SERVICE->getGridsPath() + file_name + ".grid";

				//// Serialize the grid data using the grid service
				//nlohmann::json grid_data = NIKE_MAP_SERVICE->serialize();

				//// Open the file for writing
				//std::ofstream file(grid_full_path, std::ios::out | std::ios::trunc);

				//// Check if the file opened successfully
				//if (!file.is_open()) {
				//	NIKEE_CORE_ERROR("FILE CANNOT BE OPEN");
				//}

				//// Write the serialized grid data to the file
				//file << grid_data.dump(4);
				//file.close();

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
		float top = -(grid_scale.y / 2.0f);
		float bot = (grid_scale.y / 2.0f);
		float left = -(grid_scale.x / 2.0f);
		float right = (grid_scale.x / 2.0f);

		//Convert rendersize
		ImVec2 rendersize = { render_size.x, render_size.y };

		//Convert color
		ImU32 color = IM_COL32(grid_color.r * 255, grid_color.g * 255, grid_color.b * 255, grid_color.a * 255);

		//Calculate grid render thickness
		float gird_render_thickness = grid_thickness / NIKE_CAMERA_SERVICE->getActiveCamera().zoom;

		//Add lines for grid for rows
		for (int i = 0; i <= grid_size.y; i++) {
			draw->AddLine(worldToScreen({ left, top + (cell_size.y * i) }, rendersize), worldToScreen({ right, top + (cell_size.y * i) }, rendersize), color, gird_render_thickness * fullscreen_scale.x);
		}

		//Add lines for grid for cols
		for (int j = 0; j <= grid_size.x; j++) {
			draw->AddLine(worldToScreen({ left + (cell_size.x * j), top }, rendersize), worldToScreen({ left + (cell_size.x * j) , bot }, rendersize), color, gird_render_thickness * fullscreen_scale.x);
		}

		//Render dark hue over blocked squaress
		// I changed to red so i can see clearer - lim
		for (auto const& row : NIKE_MAP_SERVICE->getGrid()) {
			for (auto const& cell : row) {
				if (cell.b_blocked) {
					draw->AddRectFilled(worldToScreen({ cell.position.x - (cell_size.x / 2.0f) + (grid_thickness / fullscreen_scale.x),  cell.position.y - (cell_size.y / 2.0f) + (grid_thickness / fullscreen_scale.x) }, rendersize),
						worldToScreen({ cell.position.x + (cell_size.x / 2.0f) - (grid_thickness / fullscreen_scale.x),  cell.position.y + (cell_size.y / 2.0f) - (grid_thickness / fullscreen_scale.x) }, rendersize), IM_COL32(255, 0, 0, 100));
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
	* Scene Management Panel
	*********************************************************************/
	std::function<void()> LevelEditor::ScenesPanel::createScenePopup(std::string const& popup_id) {
		return [this, popup_id]() {

			//Static entity name input buffer
			static std::string scn_id;

			//Get Scene text
			ImGui::Text("Enter a name for the scene without .scn:");
			if (ImGui::InputText("##Scene Name", scn_id.data(), scn_id.capacity() + 1)) {
				scn_id.resize(strlen(scn_id.c_str()));
			}

			//Add spacing
			ImGui::Spacing();

			//Display each component as a button
			if (ImGui::Button("Ok") && !scn_id.empty() && (scn_id.find(".scn") == scn_id.npos) && !NIKE_ASSETS_SERVICE->isAssetRegistered(scn_id)) {

				//Craft file path from name
				std::filesystem::path path = NIKE_PATH_SERVICE->resolvePath("Game_Assets:/Scenes");

				if (!std::filesystem::exists(path)) {
					std::filesystem::create_directories(path); // Create the directory if it doesn't exist
				}

				path /= std::string(scn_id + ".scn");

				//Reset scene
				NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::RESET, ""));

				//When user click save/create scene, grid is saved together
				tile_panel.lock()->saveGrid(scn_id);

				//Serialize new empty scene
				NIKE_SERIALIZE_SERVICE->saveSceneToFile(path.string());

				//Queue new scene
				NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, std::string(scn_id + ".scn")));

				//Reset scene id buffer
				scn_id.clear();

				//Close popup
				closePopUp(popup_id);
			}

			//Same line
			ImGui::SameLine();

			//Cancel deleting asset
			if (ImGui::Button("Cancel")) {

				//Reset scene id buffer
				scn_id.clear();

				//Close popup
				closePopUp(popup_id);
			}
			};
	}

	std::function<void()> LevelEditor::ScenesPanel::deleteScenePopup(std::string const& popup_id) {
		return [this, popup_id]() {

			//Warning message
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "This action cannot be undone!");

			//Select a component to add
			ImGui::Text("Are you sure you want to delete this scene?");

			//Add spacing
			ImGui::Spacing();

			//Display each component as a button
			if (ImGui::Button("Confirm")) {

				//Get selected asset path
				auto path = NIKE_ASSETS_SERVICE->getAssetPath(NIKE_SCENES_SERVICE->getCurrSceneID());

				// Clear containers containing entities string refs
				NIKE_ECS_MANAGER->destroyAllEntities();

				tile_panel.lock()->removeGrid(NIKE_SCENES_SERVICE->getCurrSceneID());

				//Remove path and clear selected asset text buffer
				std::filesystem::remove(path);

				// Reset grid here
				// NIKE_MAP_SERVICE->resetGrid();

				//Close popup
				closePopUp(popup_id);
			}

			//Same line
			ImGui::SameLine();

			//Cancel deleting asset
			if (ImGui::Button("Cancel")) {

				//Close popup
				closePopUp(popup_id);
			}
			};
	}

	void LevelEditor::ScenesPanel::setPopUpErrorMsg(std::string const& msg)
	{
		err_msg->assign(msg);
	}

	void LevelEditor::ScenesPanel::updateLayerNames() {
		layer_names.clear();
		for (const auto& layer : NIKE_SCENES_SERVICE->getLayers()) {
			layer_names.push_back("Layer " + std::to_string(layer->getLayerID()));
		}
	}

	void LevelEditor::ScenesPanel::init()
	{
		err_msg = std::make_shared<std::string>("Layer Fault");
		success_msg = std::make_shared<std::string>("Success");
		registerPopUp("Error", defPopUp("Error", err_msg));
		registerPopUp("Success", defPopUp("Success", success_msg));
		registerPopUp("Delete Scene", deleteScenePopup("Delete Scene"));
		registerPopUp("Create Scene", createScenePopup("Create Scene"));

		// Weak ptr ref to tile panel
		tile_panel = std::dynamic_pointer_cast<TileMapPanel>(NIKE_LVLEDITOR_SERVICE->getPanel(TileMapPanel::getStaticName()));
	}

	void LevelEditor::ScenesPanel::update()
	{
		// Empty for now, nothing to update
	}

	void LevelEditor::ScenesPanel::render()
	{
		ImGui::Begin(getName().c_str());

		//Scene ref
		ImGui::Text("Scene ID: %s", NIKE_SCENES_SERVICE->getCurrSceneID().c_str());

		//Scene ref payload
		if (ImGui::BeginDragDropTarget()) {
			//Scene file payload
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Scene_FILE")) {
				//Get asset ID
				std::string asset_id(static_cast<const char*>(payload->Data));

				//Change scene payload
				NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, asset_id));
			}
			ImGui::EndDragDropTarget();
		}

		//Create new scene
		{
			if (ImGui::Button("Create Scene")) {
				openPopUp("Create Scene");
			}
		}

		ImGui::SameLine();

		//Delete curr scene
		{
			if (ImGui::Button("Delete Scene")) {
				if (!NIKE_SCENES_SERVICE->getCurrSceneID().empty()) {
					openPopUp("Delete Scene");
				}
				else {
					err_msg->assign("No scene attached, unable to delete anything.");
					openPopUp("Error");
				}
			}
		}

		ImGui::SameLine();

		//Save curr scene
		{
			if (ImGui::Button("Save Scene")) {
				if (!NIKE_SCENES_SERVICE->getCurrSceneID().empty()) {


					std::filesystem::path scn_id = NIKE_SCENES_SERVICE->getCurrSceneID();

					// When user click save scene, grid is saved together
					tile_panel.lock()->saveGrid(scn_id);

					//Save scene
					NIKE_SERIALIZE_SERVICE->saveSceneToFile(NIKE_ASSETS_SERVICE->getAssetPath(NIKE_SCENES_SERVICE->getCurrSceneID()).string());

					success_msg->assign("Scene successfully saved.");
					openPopUp("Success");
				}
				else {
					err_msg->assign("No scene attached, create a scene before saving.");
					openPopUp("Error");
				}
			}
		}


		//Add Separator
		ImGui::Separator();

		// Get total layer count
		unsigned int layer_count = NIKE_SCENES_SERVICE->getLayerCount();

		// Get layers
		auto& layers = NIKE_SCENES_SERVICE->getLayers();

		// Update the layer names when layers change
		if (layer_count != layer_names.size()) {
			updateLayerNames();
		}

		// Display layer count
		ImGui::Text("Total Layers: %u", layer_count);

		// Layer selection dropdown
		if (!layers.empty()) {
			ImGui::Text("Select Layer");
			if (ImGui::BeginCombo("##Select Layer",
				(selected_layer_index < layers.size() ? layer_names[selected_layer_index].c_str() : "None"))) {
				for (unsigned int i = 0; i < layers.size(); ++i) {
					const bool is_selected = (selected_layer_index == i);
					if (ImGui::Selectable(layer_names[i].c_str(), is_selected)) {

						Action select_layer_action;

						// Capture the previous and new layer indices
						unsigned int prev_layer_index = selected_layer_index;
						unsigned int new_layer_index = i;

						// Do action
						select_layer_action.do_action = [&, prev_layer_index, new_layer_index]() {
							selected_layer_index = new_layer_index;
							// Reset bit position
							bit_position = 0;
							edit_mask_id = static_cast<unsigned int>(
								layers[selected_layer_index]->getLayerMask().to_ulong());
							};

						// Undo action
						select_layer_action.undo_action = [&, prev_layer_index, new_layer_index]() {
							selected_layer_index = prev_layer_index;
							// Reset bit position
							bit_position = 0;
							edit_mask_id = static_cast<unsigned int>(
								layers[selected_layer_index]->getLayerMask().to_ulong());
							};

						// Execute the action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(select_layer_action));
					}
					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		else {
			ImGui::Text("No layers available.");
		}

		// Create layer button
		if (ImGui::Button("Create Layer")) {
			if (layer_count < 64) {
				Action create_layer_action;

				// Capture the state before adding the new layer
				unsigned int previous_layer_count = static_cast<unsigned int>(layers.size());

				// Do action
				create_layer_action.do_action = [&, previous_layer_count]() {
					NIKE_SCENES_SERVICE->createLayer(previous_layer_count);
					updateLayerNames();
					};

				// Undo action
				create_layer_action.undo_action = [&, previous_layer_count]() {
					if (!layers.empty()) {
						NIKE_SCENES_SERVICE->removeLayer(previous_layer_count);
						updateLayerNames();
					}
					};

				// Execute the action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(create_layer_action));
			}
			else {
				setPopUpErrorMsg("Unable to create layer");
				openPopUp("Error");
			}
		}

		ImGui::SameLine();

		// Remove layer button
		if (ImGui::Button("Remove Layer")) {
			if (layer_count > 1) {
				Action remove_layer_action;

				// Capture state before removing layer
				auto& removed_layer = layers[selected_layer_index];
				unsigned int removed_layer_id = removed_layer->getLayerID();

				// Do action
				remove_layer_action.do_action = [&, removed_layer_id]() {
					NIKE_SCENES_SERVICE->removeLayer(removed_layer_id);
					// Adjusts selected_layer_index to the last valid index
					selected_layer_index = min(selected_layer_index, static_cast<unsigned int>(layers.size() - 1));
					updateLayerNames();
					};

				// Undo action
				remove_layer_action.undo_action = [&, removed_layer_id]() {
					NIKE_SCENES_SERVICE->createLayer(removed_layer_id);
					updateLayerNames();
					};

				// Execute the action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(remove_layer_action));
			}
			else {
				setPopUpErrorMsg("Unable to remove layer");
				openPopUp("Error");
			}
		}

		ImGui::Separator();

		// Show layer editing options
		if (selected_layer_index < layers.size()) {
			ImGui::Text("Edit Layer Mask");

			// Layer mask editing
			if (layers.size() > 1) {

				// For ensuring bit_position does not default to the newly created layer
				if (bit_position >= layers.size() || bit_position == selected_layer_index) {
					bit_position = (selected_layer_index == 0) ? 1 : 0;
				}

				if (ImGui::BeginCombo("##Select Mask Layer", layer_names[bit_position].c_str())) {
					for (unsigned int i = 0; i < layers.size(); ++i) {
						if (i == selected_layer_index) continue;

						const bool mask_selected = (bit_position == i);
						if (ImGui::Selectable(layer_names[i].c_str(), mask_selected)) {
							bit_position = i;
						}
						if (mask_selected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				bit_state = layers[selected_layer_index]->getLayerMask().test(bit_position);
				if (ImGui::Checkbox("Set Bit State", &bit_state)) {
					layers[selected_layer_index]->setLayerMask(bit_position, bit_state);
				}
			}
			else {
				ImGui::Text("No mask layers available.");
			}
		}


		//Render popups
		renderPopUps();

		ImGui::End();
	}

	/*****************************************************************//**
	* Game Window Panel
	*********************************************************************/
	void LevelEditor::GameWindowPanel::renderAcceptPayload() {
		if (ImGui::BeginDragDropTarget()) {

			//Calculate render position
			Vector2f render_pos;
			if (tile_map_panel.lock()->checkGridSnapping()) {
				//Get snapped to cell position
				auto cursor_cell = NIKE_MAP_SERVICE->getCellAtPosition(Vector2f(world_mouse_pos.x, -world_mouse_pos.y));
				if (cursor_cell.has_value()) {

					//Snap to cell
					render_pos = cursor_cell.value().get().position;
				}
				else {
					render_pos = { world_mouse_pos.x, -world_mouse_pos.y };
				}
			}
			else {
				render_pos = { world_mouse_pos.x, -world_mouse_pos.y };
			}

			//Texture file payload
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture_FILE")) {
				//Get asset ID
				std::string asset_id(static_cast<const char*>(payload->Data));

				//Get texture asset
				auto texture = NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(asset_id);

				// Define an undo/redo action
				Action drag_drop_action;

				// For lamda to use
				auto entity_id = std::make_shared<Entity::Type>();

				//Do Action
				drag_drop_action.do_action = [entity_id, texture, asset_id, render_pos]() {
					//Creat new entity 
					*entity_id = NIKE_ECS_MANAGER->createEntity(NIKE_SCENES_SERVICE->getLayerCount() - 1);

					//Add transform
					NIKE_ECS_MANAGER->addEntityComponent<Transform::Transform>(*entity_id, Transform::Transform(render_pos, Vector2f((float)texture->size.x, (float)texture->size.y), 0.0f));

					//Add texture
					NIKE_ECS_MANAGER->addEntityComponent<Render::Texture>(*entity_id, Render::Texture(asset_id, { 0.0f, 0.0f, 0.0f, 1.0f }));
					};

				//Undo Action
				drag_drop_action.undo_action = [entity_id]() {

					//Check if entity is still alive
					if (NIKE_ECS_MANAGER->checkEntity(*entity_id)) {
						//Destroy new entity
						NIKE_ECS_MANAGER->destroyEntity(*entity_id);
					}
					};

				// Execute the action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(drag_drop_action));


			}

			//Model file payload
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Model_FILE")) {
				//Get asset ID
				std::string asset_id(static_cast<const char*>(payload->Data));

				//Get model asset
				auto model = NIKE_ASSETS_SERVICE->getAsset<Assets::Model>(asset_id);

				//Default model size
				Vector2f size = { 100.0f, 100.0f };

				//Default model color
				Vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f };

				// Define an undo/redo action
				Action drag_drop_action;

				// For lamda to use
				auto entity_id = std::make_shared<Entity::Type>();

				//Do Action
				drag_drop_action.do_action = [entity_id, size, color, asset_id, render_pos]() {
					//Creat new entity 
					*entity_id = NIKE_ECS_MANAGER->createEntity(NIKE_SCENES_SERVICE->getLayerCount() - 1);

					//Add transform
					NIKE_ECS_MANAGER->addEntityComponent<Transform::Transform>(*entity_id, Transform::Transform(render_pos, size, 0.0f));

					//Add model
					NIKE_ECS_MANAGER->addEntityComponent<Render::Shape>(*entity_id, Render::Shape(asset_id, color));
					};

				//Undo Action
				drag_drop_action.undo_action = [entity_id]() {

					//Check if entity is still alive
					if (NIKE_ECS_MANAGER->checkEntity(*entity_id)) {
						//Destroy new entity
						NIKE_ECS_MANAGER->destroyEntity(*entity_id);
					}
					};

				// Execute the action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(drag_drop_action));
			}

			//Texture font payload
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Font_FILE")) {
				//Get asset ID
				std::string asset_id(static_cast<const char*>(payload->Data));

				//Get font asset
				auto font = NIKE_ASSETS_SERVICE->getAsset<Assets::Font>(asset_id);

				//Default font color
				Vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f };

				//Default place holder text
				std::string place_holder{ "New text" };

				// For lamda to use
				auto entity_id = std::make_shared<Entity::Type>();

				// Define an undo/redo action
				Action drag_drop_action;

				// For lamda to capture
				Vector2f local_world_mouse_pos = getWorldMousePos();

				//Do Action
				drag_drop_action.do_action = [entity_id, color, asset_id, place_holder, local_world_mouse_pos]() {
					//Creat new entity 
					*entity_id = NIKE_ECS_MANAGER->createEntity(NIKE_SCENES_SERVICE->getLayerCount() - 1);

					//Add transform
					NIKE_ECS_MANAGER->addEntityComponent<Transform::Transform>(*entity_id, Transform::Transform(Vector2f(local_world_mouse_pos.x, -local_world_mouse_pos.y), Vector2f(0.0f, 0.0f), 0.0f));

					//Add texture
					NIKE_ECS_MANAGER->addEntityComponent<Render::Text>(*entity_id, Render::Text(asset_id, place_holder, color, 1.0f));
					};

				//Undo Action
				drag_drop_action.undo_action = [entity_id]() {

					//Check if entity is still alive
					if (NIKE_ECS_MANAGER->checkEntity(*entity_id)) {
						//Destroy new entity
						NIKE_ECS_MANAGER->destroyEntity(*entity_id);
					}
					};

				// Execute the action
				NIKE_LVLEDITOR_SERVICE->executeAction(std::move(drag_drop_action));
			}

			//Scene file payload
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Scene_FILE")) {
				//Get asset ID
				std::string asset_id(static_cast<const char*>(payload->Data));

				//Change scene payload
				NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, asset_id));
			}

			ImGui::EndDragDropTarget();
		}
	}

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

		//Configure UV Offsets
		ImVec2 uv0(u_min, -v_min); // Bottom-left
		ImVec2 uv1(u_max, -v_max); // Top-right

		//Render game to viewport
		ImGui::Image((ImTextureID)texture_id, ImVec2(viewport_width, viewport_height), uv0, uv1);

		//Accept render assets payload
		renderAcceptPayload();

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
