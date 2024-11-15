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
		//Set up main panel position for docking
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x),
			static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y)));
	}

	void LevelEditor::MainPanel::render() {

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
	void LevelEditor::EntitiesPanel::init() {
	}

	void LevelEditor::EntitiesPanel::update() {
	}

	void LevelEditor::EntitiesPanel::render() {
		ImGui::Begin(getName().c_str());

		ImGui::End();
	}
}
