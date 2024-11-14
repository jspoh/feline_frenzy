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

namespace NIKE {

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
}
