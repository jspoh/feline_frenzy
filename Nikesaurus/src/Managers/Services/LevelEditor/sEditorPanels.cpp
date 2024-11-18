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
		ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
		ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
		ImVec2 popupPos = ImVec2(viewportPos.x + viewportSize.x * 0.5f, viewportPos.y + viewportSize.y * 0.5f);

		//Center the popup
		ImGui::SetNextWindowPos(popupPos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

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

		//Actual Window Size
		Vector2i win_size = NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize();

		//Configure viewport size
		ImVec2 window_size = ImGui::GetContentRegionAvail();
		float aspect_ratio = (float)win_size.x / (float)win_size.y;
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

					//Save entity name into entities ref
					entity_to_name.emplace(new_id, shared_id->c_str());
					name_to_entity.emplace(shared_id->c_str(), new_id);

					//Update entities list
					entities = std::move(NIKE_ECS_MANAGER->getAllEntities());

					//Set selected entity back to old entity
					selected_entity = name_to_entity.at(shared_id->data());
				};

				//Setup action for removing entity
				remove.do_action = [&, shared_id]() {

					//Check if entity is still alive
					if (name_to_entity.find(shared_id->data()) != name_to_entity.end()) {
						//Destroy entity
						NIKE_ECS_MANAGER->destroyEntity(name_to_entity.at(shared_id->data()));

						//Erase new entity ref
						entity_to_name.erase(name_to_entity.at(shared_id->data()));
						name_to_entity.erase(shared_id->data());

						//Update entities list
						entities = std::move(NIKE_ECS_MANAGER->getAllEntities());

						//Set selected entity back to first entity
						selected_entity = entities.empty() ? 0 : *entities.begin();
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
				clone.do_action = [&, shared_id, clone]() {
					if (NIKE_ECS_MANAGER->checkEntity(clone_entity)) {
						//Clone entity 
						Entity::Type new_id = NIKE_ECS_MANAGER->cloneEntity(clone_entity);

						//Save entity name into entities ref
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
	}

	void LevelEditor::EntitiesPanel::update() {

		//Get all active entities
		entities = std::move(NIKE_ECS_MANAGER->getAllEntities());

		//Check if number of refs matches the active entities size
		if (entities.size() != entity_to_name.size()) {

			//Iterate through all active entities
			unsigned int count = 0;
			for (auto entity : entities) {
				//Check if entity ref has already been added
				if (entity_to_name.find(entity) == entity_to_name.end()) {

					//Create identifier for entity
					char entity_name[32];
					snprintf(entity_name, sizeof(entity_name), "entity_%04d", count);

					//Add entity to BiMap
					entity_to_name.emplace(entity, entity_name);
					name_to_entity.emplace(entity_name, entity);
				}

				++count;
			}

			//Extra check for if entities were removed
			if (entities.size() != entity_to_name.size()) {

				//Iterate through entities ref to check which entity has been removed
				auto it = entity_to_name.begin();
				auto rit = name_to_entity.begin();
				for (;it != entity_to_name.end();) {

					//Find entity
					auto active_it = entities.find(it->first);
					//Check if entity is still active
					if (active_it == entities.end()) {

						//Add entity to map of ref
						it = entity_to_name.erase(it);
						rit = name_to_entity.erase(rit);
					}
					else {
						rit++;
						it++;
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
		if (ImGui::Button("Remove") && (entities.find(selected_entity) != entities.end()) && !entities.empty()) {
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

		//Check if there are entities present
		if (entities.empty()) {
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<SelectedEntityEvent>((Entity::Type)0, std::string("")));
		}
		else {
			//Iterate through all active entities
			for (auto entity : entities) {

				//Check if entity is selected
				bool selected = (entities.find(selected_entity) != entities.end()) && entity_to_name.at(entity).c_str() == entity_to_name.at(selected_entity).c_str();

				//Show selectable
				if (ImGui::Selectable(entity_to_name.at(entity).c_str(), selected)) {
					selected_entity = entity;

					//Get selected entity data
					auto it = entity_to_name.find(selected_entity);

					//Dispatch new selected entity event
					NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<SelectedEntityEvent>(it->first, it->second));
				}
			}
		}

		//Render popups
		renderPopUps();

		ImGui::End();
	}

	/*****************************************************************//**
	* Components Panel
	*********************************************************************/
	void LevelEditor::ComponentsPanel::onEvent(std::shared_ptr<SelectedEntityEvent> event) {
		selected_entity = event->selected_entity;
		selected_entity_ref = event->selected_entity_ref;
		event->setEventProcessed(true);
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
				if (NIKE_ECS_MANAGER->checkEntityComponent(selected_entity, component.second))
					continue;

				//Display each component as a button
				if (ImGui::Button(component.first.c_str())) {

					//Temporary add component action
					Action add_comp;

					//Setup undo action for add component
					add_comp.undo_action = [=]() {
						if (NIKE_ECS_MANAGER->checkEntityComponent(selected_entity, component.second)) {
							NIKE_ECS_MANAGER->removeEntityComponent(selected_entity, component.second);
						}
					};

					//Setup do action for add component
					add_comp.do_action = [=]() {

						//Add default comp to entity
						NIKE_ECS_MANAGER->addDefEntityComponent(selected_entity, component.second);
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

	void LevelEditor::ComponentsPanel::init() {

		//Setup event listening for selected entity
		std::shared_ptr<LevelEditor::ComponentsPanel> comp_panel_wrapped(this, [](LevelEditor::ComponentsPanel*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<SelectedEntityEvent>(comp_panel_wrapped);

		//Register add component popup
		registerPopUp("Add Component", addComponentPopUp("Add Component"));
		error_msg = std::make_shared<std::string>("Comp Error");
		registerPopUp("Error", defPopUp("Error", error_msg));
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

		//Check if an entity has been selected
		if (NIKE_ECS_MANAGER->checkEntity(selected_entity)) {

			//Print out selected entity string ref
			ImGui::Text("Selected Entity: %s", selected_entity_ref.c_str());

			//Print out selected entity component count
			ImGui::Text("Number of Components in entity: %d", NIKE_ECS_MANAGER->getEntityComponentCount(selected_entity));

			//Print out selected entity layer id
			ImGui::Text("Entity's Layer: %d", NIKE_ECS_MANAGER->getEntityLayerID(selected_entity));

			//Add Spacing
			ImGui::Spacing();

			//Add component popup
			if (ImGui::Button("Add Component")) {
				openPopUp("Add Component");
			}

			//Add Spacing
			ImGui::Spacing();

			//Retrieve and display all registered component types
			for (auto& comp : NIKE_ECS_MANAGER->getAllEntityComponents(selected_entity)) {

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
						Entity::Type entity_copy = selected_entity;
						auto comp_copy = NIKE_ECS_MANAGER->getCopiedEntityComponent(selected_entity, comp_type_copy);

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
			ImGui::Text("No active entities.");
		}

		//Render popups
		renderPopUps();

		ImGui::End();
	}

	void LevelEditor::ComponentsPanel::setPopUpErrorMsg(std::string const& msg) {
		error_msg->assign(msg);
	}
}
