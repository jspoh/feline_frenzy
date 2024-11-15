/*****************************************************************//**
 * \file   sEditorPanels.cpp
 * \brief  Level Editor panel implementation
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/LevelEditor/sEditorPanels.h"
#include "Core/Engine.h"
#include "Systems/Render/sysRender.h"

namespace NIKE {
	/*****************************************************************//**
	* Panel Interface
	*********************************************************************/
	void LevelEditor::IPanel::init() {
	}

	void LevelEditor::IPanel::registerPopUp(std::string const& popup_id, std::function<void()> popup_func) {
		popups[popup_id] = PopUp{ false, popup_func };
	}

	void LevelEditor::IPanel::openPopUp(std::string const& popup_id) {
		auto it = popups.find(popup_id);

		if (it == popups.end()) {
			throw std::runtime_error("Popup doest not exist");
		}

		popups.at(popup_id).b_is_open = true;
		ImGui::OpenPopup(popup_id.c_str());
	}

	void LevelEditor::IPanel::closePopUp(std::string const& popup_id) {
		auto it = popups.find(popup_id);

		if (it == popups.end()) {
			throw std::runtime_error("Popup doest not exist");
		}

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

	std::function<void()> LevelEditor::IPanel::errorPopUp(std::string const& error_id, std::string const& error_msg) {
		return [this, error_id, error_msg]() {
			//Show error message
			ImGui::Text("%s", error_msg.c_str());

			//Add Spacing
			ImGui::Spacing();

			//OK button to close the popup
			if (ImGui::Button("OK")) {
				closePopUp(error_id);
			}
		};
	}

	/*****************************************************************//**
	* Main Panel
	*********************************************************************/
	void LevelEditor::MainPanel::setDebugState(bool state) {
		b_debug_mode = state;
	}

	bool LevelEditor::MainPanel::getDebugState() const {
		return b_debug_mode;
	}

	void LevelEditor::MainPanel::setGameState(bool state) {
		b_game_state = state;

		//Get all ecs systems
		auto& systems = NIKE_ECS_MANAGER->getAllSystems();

		//Set the state of each systems based on new game state
		if (b_game_state) {
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

	void LevelEditor::MainPanel::init() {

		//Setup window flags
		window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	void LevelEditor::MainPanel::update() {
	}

	void LevelEditor::MainPanel::render() {
		//Set up main panel position for docking
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x),
			static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y)));

		//Begin Frame
		ImGui::Begin(getName().c_str(), nullptr, static_cast<ImGuiWindowFlags>(window_flags));

		//Start Main Panel Menu Bar
		if (ImGui::BeginMenuBar()) {

			//Game State Switching
			ImGui::Spacing();
			ImGui::Text("Play / Pause Controls : ");
			if (ImGui::Button(b_game_state ? "Play" : "Pause")) {
				b_game_state = !b_game_state;
			}

			//Debug Mode Switching
			ImGui::Spacing();
			ImGui::Text("Toggle Debug Mode : ");
			if (ImGui::Button(b_debug_mode ? "Show" : "Hide")) {
				b_debug_mode = !b_debug_mode;
			}

			//Reset Scene
			ImGui::Spacing();
			ImGui::Text("Reset Scene :");
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
	* Game Window Panel
	*********************************************************************/
	void LevelEditor::GameWindowPanel::onEvent(std::shared_ptr<Render::ViewportTexture> event) {
		texture_id = event->tex_id;
		event->setEventProcessed(true);
	}

	Vector2f LevelEditor::GameWindowPanel::getRelativeMousePos() const {
		return relative_mouse_pos;
	}

	bool LevelEditor::GameWindowPanel::isMouseInWindow() const {
		//Get window size
		Vector2i win_size = NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize();

		//Check if relative cursor is within window size
		if (relative_mouse_pos.x >= 0.0f &&
			relative_mouse_pos.y >= 0.0f &&
			relative_mouse_pos.x <= static_cast<float>(win_size.x) &&
			relative_mouse_pos.y <= static_cast<float>(win_size.y)) 
		{
			return true;
		}
		else {
			return false;
		}
	}

	void LevelEditor::GameWindowPanel::init() {
		std::shared_ptr<GameWindowPanel> game_window_listener (this, [](GameWindowPanel*){});
		NIKE_EVENTS_SERVICE->addEventListeners<Render::ViewportTexture>(game_window_listener);
	}

	void LevelEditor::GameWindowPanel::update() {
	}

	void LevelEditor::GameWindowPanel::render() {
		ImGui::Begin(getName().c_str());

		//Configure viewport size
		ImVec2 window_size = ImGui::GetContentRegionAvail();
		float aspect_ratio = 16.0f / 9.0f;
		float viewport_width = window_size.x;
		float viewport_height = window_size.x / aspect_ratio;
		if (viewport_height > window_size.y) {
			viewport_height = window_size.y;
			viewport_width = window_size.y * aspect_ratio;
		}

		//Get mous input
		ImGuiIO& io = ImGui::GetIO();

		//Get the position of the game window top-left corner
		ImVec2 gameWindowPos = ImGui::GetCursorScreenPos();

		//Actual Window Size
		Vector2i win_size = NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize();

		//Scale Factor
		Vector2f scale{ (float)win_size.x / viewport_width, (float)win_size.y / viewport_height };

		//Calculate mouse position relative to the game window
		relative_mouse_pos = { (io.MousePos.x - gameWindowPos.x) * scale.x, (io.MousePos.y - gameWindowPos.y) * scale.y };

		//Configure UV Offsets
		ImVec2 uv0(0.0f, 1.0f); // Bottom-left
		ImVec2 uv1(1.0f, 0.0f); // Top-right

		//Render game to viewport
		ImGui::Image((ImTextureID)texture_id, ImVec2(viewport_width, viewport_height), uv0, uv1);

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
				//Creat new entity 
				Entity::Type new_id = NIKE_ECS_MANAGER->createEntity(layer_id);
					
				//If entity name is not provided (Create a default)
				if (entity_name.empty())
				{
					snprintf(entity_name.data(), entity_name.capacity() + 1, "entity_%04d", new_id);
				}

				//Save entity name into entities ref
				entities_ref.insert({ new_id, entity_name });

				//Reset entity name
				entity_name.clear();

				//Close popup
				closePopUp(popup_id);
			}

			ImGui::SameLine();

			//Cancel creating new entity
			if (ImGui::Button("Cancel")) {

				//Close popup
				closePopUp(popup_id);
			}
		};
	}

	std::function<void()> LevelEditor::EntitiesPanel::removeEntityPopUp(std::string const& popup_id) {
		return [this, popup_id]() {

			//Confirm removal of entity
			ImGui::Text("Are you sure you want to remove %s?", entities_ref.at(selected_entity).c_str());

			//If enter or ok button is pressed
			if (ImGui::Button("Remove") || ImGui::GetIO().KeysDown[NIKE_KEY_ENTER]) {

				//Destroy entity
				NIKE_ECS_MANAGER->destroyEntity(selected_entity);

				//Remove selected entity ref
				entities_ref.erase(selected_entity);

				//Update entities list
				entities = std::move(NIKE_ECS_MANAGER->getAllEntities());

				//Set selected entity back to first entity
				selected_entity = entities.empty() ? 0 : *entities.begin();

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

	void LevelEditor::EntitiesPanel::init() {

		//Register popups
		registerPopUp("Create Entity", createEntityPopUp("Create Entity"));
		registerPopUp("Remove Entity", removeEntityPopUp("Remove Entity"));

		//Init active entities
		entities = std::move(NIKE_ECS_MANAGER->getAllEntities());

		//Skip initialization if entities are empty
		if (entities.empty())
			return;

		//Iterate through all active entities to init entities ref
		std::for_each(entities.begin(), entities.end(),
			[this](Entity::Type entity) {

				//Check if entity ref has already been added
				if (entities_ref.find(entity) == entities_ref.end()) {

					//Create identifier for entity
					char entity_name[32];
					snprintf(entity_name, sizeof(entity_name), "entity_%04d", entity);

					//Add entity to map of ref
					entities_ref.insert({ entity, entity_name });
				}
			});

		//Init selected entity
		selected_entity = *entities.begin();
	}

	void LevelEditor::EntitiesPanel::update() {

		//Get all active entities
		entities = std::move(NIKE_ECS_MANAGER->getAllEntities());

		//Check if number of refs matches the active entities size
		if (entities.size() != entities_ref.size()) {

			//Iterate through all active entities
			std::for_each(entities.begin(), entities.end(), 
				[this](Entity::Type entity) {

					//Check if entity ref has already been added
					if (entities_ref.find(entity) == entities_ref.end()) {

						//Create identifier for entity
						char entity_name[32];
						snprintf(entity_name, sizeof(entity_name), "entity_%04d", entity);

						//Add entity to map of ref
						entities_ref.insert({ entity, entity_name });
					}
				});

			//Extra check for if entities were removed
			if (entities.size() != entities_ref.size()) {

				//Iterate through entities ref to check which entity has been removed
				for (auto it = entities_ref.begin(); it != entities_ref.end();) {

					//Find entity
					auto active_it = entities.find(it->first);
					//Check if entity is still active
					if (active_it == entities.end()) {

						//Add entity to map of ref
						it = entities_ref.erase(it);
					}
					else {
						it++;
					}
				}
			}
		}
	}

	void LevelEditor::EntitiesPanel::render() {
		ImGui::Begin(getName().c_str());

		// Button to create an entity, which triggers the popup
		if (ImGui::Button("Create Entity") && entities.size() < Entity::MAX) {
			openPopUp("Create Entity");
		}

		//Buttons Same Line
		ImGui::SameLine();

		// Button to remove an entity, which triggers the popup
		if (ImGui::Button("Remove Entity") && !entities.empty()) {
			openPopUp("Remove Entity");
		}

		//Add Spacing
		ImGui::Spacing();

		//Show number of entities in the level
		ImGui::Text("Number of entities in level: %d", entities.size());

		//Add Spacing
		ImGui::Spacing();

		//Iterate through all active entities
		for (auto entity : entities) {

			//Check if entity is selected
			bool selected = (entities.find(selected_entity) != entities.end()) && entities_ref.at(entity).c_str() == entities_ref.at(selected_entity).c_str();
			
			//Show selectable
			if (ImGui::Selectable(entities_ref.at(entity).c_str(), selected)) {
				selected_entity = entity;
			}
		}

		//Render popups
		renderPopUps();

		ImGui::End();
	}
}
