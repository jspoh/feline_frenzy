/*****************************************************************//**
 * \file   sLevelEditor.h
 * \brief  Level editor
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/sLevelEditor.h"
#include "Core/Engine.h"

namespace NIKE {
	void LevelEditor::Service::beginFrame() {
		// Start a new ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void LevelEditor::Service::endFrame() {
		// Render ImGui frame
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void LevelEditor::Service::init() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(std::static_pointer_cast<Windows::NIKEWindow>(NIKE_WINDOWS_SERVICE->getWindow())->getWindowPtr(), true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	void LevelEditor::Service::update() {

		////Toggle editor active
		//if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_TAB)) {
		//	b_editor_active = !b_editor_active;
		//	//b_dispatch_viewport = true;
		//}

		//Update all panels
		for (auto& panel : panels) {
			panel.second->update();
		}
	}

	void LevelEditor::Service::render() {
		beginFrame();

		//Render all panels
		for (auto& panel : panels) {
			panel.second->render();
		}

		endFrame();
	}

	void LevelEditor::Service::cleanUp() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void LevelEditor::Service::setEditorState(bool state) {
		b_editor_active = state;
	}

	void LevelEditor::Service::setDebugState(bool state) {
		b_debug_mode = state;
	}

	void LevelEditor::Service::setGameState(bool state) {
		b_game_state = state;

		if (b_game_state) {
			////Set audio system pause
			//NIKE_ECS_MANAGER->setSystemState<Audio::Manager>(false);

			////Set physics system pause
			//NIKE_ECS_MANAGER->setSystemState<Physics::Manager>(false);

			////Set animation system pause
			//NIKE_ECS_MANAGER->setSystemState<Animation::Manager>(false);
		}
		else {
			////Set audio system play
			//NIKE_ECS_MANAGER->setSystemState<Audio::Manager>(true);

			////Set physics system play
			//NIKE_ECS_MANAGER->setSystemState<Physics::Manager>(true);

			////Set animation system play
			//NIKE_ECS_MANAGER->setSystemState<Animation::Manager>(true);
		}
	}

	void LevelEditor::Service::addPanel(const std::string& panel_id, std::shared_ptr<LevelEditor::IPanel> panel) {
		auto it = panels.find(panel_id);
		if (it != panels.end()) {
			throw std::runtime_error("Panel already added.");
		}

		panels.insert({ panel_id, panel });
	}

	void LevelEditor::Service::removePanel(const std::string& panel_id) {
		auto it = panels.find(panel_id);
		if (it == panels.end()) {
			throw std::runtime_error("Panel doest not exist.");
		}

		panels.erase(it);
	}
}
