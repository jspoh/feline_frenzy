/*****************************************************************//**
 * \file   sLevelEditor.h
 * \brief  Level editor
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/LevelEditor/sLevelEditor.h"
#include "Core/Engine.h"
#include "Systems/Render/sysRender.h"

namespace NIKE {
	// Mapping function (example)
	int LevelEditor::Service::mapCodeToIMGUICode(int code) {

		//Code within 0 - 9 range
		if (code >= NIKE_KEY_0 && code <= NIKE_KEY_9) {
			return ImGuiKey_0 + (code - NIKE_KEY_0);
		}
		//Code within A - Z range
		else if (code >= NIKE_KEY_A && code <= NIKE_KEY_Z) {
			return ImGuiKey_A + (code - NIKE_KEY_A);
		}
		//Code within F1 - F24 range
		else if(code >= NIKE_KEY_F1 && code <= NIKE_KEY_F24) {
			return ImGuiKey_F1 + (code - NIKE_KEY_F1);
		}
		//Code within KeyPad_0 - KeyPad_Equal range
		else if (code >= NIKE_KEY_KP_0 && code <= NIKE_KEY_KP_EQUAL) {
			return ImGuiKey_Keypad0 + (code - NIKE_KEY_KP_0);
		}
		//Code within Apostrophe - Slash range
		else if (code >= NIKE_KEY_APOSTROPHE && code <= NIKE_KEY_SLASH) {
			return ImGuiKey_Apostrophe + (code - NIKE_KEY_APOSTROPHE);
		}
		//Code within left bracket & grave accent
		else if (code >= NIKE_KEY_LEFT_BRACKET && code <= NIKE_KEY_GRAVE_ACCENT) {
			return ImGuiKey_LeftBracket + (code - NIKE_KEY_LEFT_BRACKET);
		}
		//Code within caps lock & pause
		else if (code >= NIKE_KEY_CAPS_LOCK && code <= NIKE_KEY_PAUSE) {
			return ImGuiKey_CapsLock + (code - NIKE_KEY_CAPS_LOCK);
		}
		//Code within caps lock & pause
		else if (code >= NIKE_KEY_PAGE_UP && code <= NIKE_KEY_END) {
			return ImGuiKey_PageUp + (code - NIKE_KEY_PAGE_UP);
		}

		//Mouse button codes
		switch (code) {
		case NIKE_MOUSE_BUTTON_LEFT: return ImGuiKey_MouseLeft;
		case NIKE_MOUSE_BUTTON_RIGHT: return ImGuiKey_MouseRight;
		case NIKE_MOUSE_BUTTON_MIDDLE: return ImGuiKey_MouseMiddle;
		case NIKE_MOUSE_BUTTON_4: return ImGuiKey_MouseX1;
		case NIKE_MOUSE_BUTTON_5: return ImGuiKey_MouseX2;
		}
		
		//Remaing key codes that are not in order
		switch (code) {
		case NIKE_KEY_SPACE: return ImGuiKey_Space;
		case NIKE_KEY_SEMICOLON: return ImGuiKey_Semicolon;
		case NIKE_KEY_EQUAL: return ImGuiKey_Equal;
		case NIKE_KEY_ESCAPE: return ImGuiKey_Escape;
		case NIKE_KEY_ENTER: return ImGuiKey_Enter;
		case NIKE_KEY_TAB: return ImGuiKey_Tab;
		case NIKE_KEY_BACKSPACE: return ImGuiKey_Backspace;
		case NIKE_KEY_INSERT: return ImGuiKey_Insert;
		case NIKE_KEY_DELETE: return ImGuiKey_Delete;
		case NIKE_KEY_RIGHT: return ImGuiKey_RightArrow;
		case NIKE_KEY_LEFT: return ImGuiKey_LeftArrow;
		case NIKE_KEY_DOWN: return ImGuiKey_DownArrow;
		case NIKE_KEY_UP: return ImGuiKey_UpArrow;
		case NIKE_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
		case NIKE_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
		case NIKE_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
		case NIKE_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
		case NIKE_KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
		case NIKE_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
		case NIKE_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
		case NIKE_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
		case NIKE_KEY_MENU: return ImGuiKey_Menu;
		}

		return ImGuiKey_None;
	}

	void LevelEditor::Service::onEvent(std::shared_ptr<Input::KeyEvent> event) {

		//Get input to set
		ImGuiIO& io = ImGui::GetIO();

		//Check if imgui wants to capture keyboard events
		if (io.WantCaptureKeyboard) {

			//Process key event
			switch (event->state) {
				case Input::States::PRESS: {
					io.KeysDown[mapCodeToIMGUICode(event->code)] = true;
					break;
				}
				case Input::States::REPEAT: {
					io.KeysDown[mapCodeToIMGUICode(event->code)] = true;
					break;
				}
				case Input::States::RELEASE: {
					io.KeysDown[mapCodeToIMGUICode(event->code)] = false;
					break;
				}
				default: {
					break;
				}
			}

			//Update modifier keys
			io.KeyShift = io.KeysDown[ImGuiKey_LeftShift] || io.KeysDown[ImGuiKey_RightShift];
			io.KeyCtrl = io.KeysDown[ImGuiKey_LeftCtrl] || io.KeysDown[ImGuiKey_RightCtrl];
			io.KeyAlt = io.KeysDown[ImGuiKey_LeftAlt] || io.KeysDown[ImGuiKey_RightAlt];
			io.KeySuper = io.KeysDown[ImGuiKey_LeftSuper] || io.KeysDown[ImGuiKey_RightSuper];

			//Mark event as processed
			event->setEventProcessed(true);
		}
	}

	void LevelEditor::Service::onEvent(std::shared_ptr<Input::MouseBtnEvent> event) {
		//Get input to set
		ImGuiIO& io = ImGui::GetIO();

		//Check if imgui wants to capture mouse events
		if (io.WantCaptureMouse) {

			//Process key event
			switch (event->state) {
			case Input::States::PRESS: {
				io.KeysDown[mapCodeToIMGUICode(event->code)] = true;
				break;
			}
			case Input::States::REPEAT: {
				io.KeysDown[mapCodeToIMGUICode(event->code)] = true;
				break;
			}
			case Input::States::RELEASE: {
				io.KeysDown[mapCodeToIMGUICode(event->code)] = false;
				break;
			}
			default: {
				break;
			}
			}

			//Mark event as processed
			event->setEventProcessed(true);
		}
	}

	void LevelEditor::Service::onEvent(std::shared_ptr<Input::MouseMovedEvent> event) {
		//Get input to set
		ImGuiIO& io = ImGui::GetIO();

		//Check if imgui wants to capture mouse events
		if (io.WantCaptureMouse) {

			//Update imgui mouse position
			io.MousePos.x = event->pos.x;
			io.MousePos.y = event->pos.y;

			//Check if current mouse pos is within game window
			if (false) {
				event->pos = translateMouseToGameWindow();
			}
			else {
				//Mark event as processed
				event->setEventProcessed(true);
			}
		}
	}

	void LevelEditor::Service::onEvent(std::shared_ptr<Input::MouseScrollEvent> event) {
		//Get input to set
		ImGuiIO& io = ImGui::GetIO();

		//Check if imgui wants to capture mouse events
		if (io.WantCaptureMouse) {

			//Update scroll offset
			io.MouseWheelH += static_cast<float>(event->offset.x);
			io.MouseWheel += static_cast<float>(event->offset.y);

			//Mark event as processed
			event->setEventProcessed(true);
		}
	}

	void LevelEditor::Service::onEvent(std::shared_ptr<Windows::WindowResized> event) {
		//Get input to set
		ImGuiIO& io = ImGui::GetIO();

		//Update imgui that windows is resized
		io.DisplaySize.x = static_cast<float>(event->frame_buffer.x);
		io.DisplaySize.y = static_cast<float>(event->frame_buffer.y);
	}

	void LevelEditor::Service::beginFrame() {
		//Start a new ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void LevelEditor::Service::endFrame() {
		//Render ImGui frame
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	Vector2f LevelEditor::Service::translateMouseToGameWindow() {
		//Insert implementation

		return Vector2f();
	}

	void LevelEditor::Service::updateShortCuts() {
		ImGuiIO& io = ImGui::GetIO();

		//Toggle Level Editor On & Off
		if (io.KeysDown[ImGuiKey_Tab]) {
			b_editor_active = !b_editor_active;
		}

		//Undo
		if (io.KeyCtrl && io.KeysDown[ImGuiKey_Z]) {
			action_manager->undo();
		}

		//Redo
		if (io.KeyCtrl && io.KeysDown[ImGuiKey_Y]) {
			action_manager->redo();
		}
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

		//Update imgui with proper delta time
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = NIKE_WINDOWS_SERVICE->getDeltaTime();

		//Update shortcuts
		updateShortCuts();

		//Return when imgui is not active
		if (!b_editor_active) {
			io.WantCaptureKeyboard = false;
			io.WantCaptureMouse = false;
			io.WantCaptureMouseUnlessPopupClose = false;
			io.WantSetMousePos = false;
			return;
		}

		//Update all panels
		for (auto& panel : panels) {
			panel.second->update();
		}
	}

	void LevelEditor::Service::render() {
		//Return when imgui is not active
		if (!b_editor_active) {
			return;
		}

		//Begin render frame
		beginFrame();

		//Render all panels
		for (auto& panel : panels) {
			panel.second->render();
		}

		//End render frame
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

	bool LevelEditor::Service::getEditorState() const {
		return b_editor_active;
	}

	void LevelEditor::Service::setDebugState(bool state) {
		b_debug_mode = state;
	}

	bool LevelEditor::Service::getDebugState() const {
		return b_debug_mode;
	}

	void LevelEditor::Service::setGameState(bool state) {
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

	bool LevelEditor::Service::getGameState() const {
		return b_game_state;
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

	void LevelEditor::Service::executeAction(Action const& action) {
		action_manager->executeAction(action);
	}
}
