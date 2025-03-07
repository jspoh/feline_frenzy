/*****************************************************************//**
 * \file   sLevelEditor.h
 * \brief  Level editor
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/LevelEditor/sLevelEditor.h"
#include "Core/Engine.h"
#include "Systems/Render/sysRender.h"

namespace NIKE {

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

		//Check if editor is active
		if (!b_editor_active)
			return;

		//Get input to set
		ImGuiIO& io = ImGui::GetIO();

		//Check if imgui wants to capture keyboard events
		if (io.WantCaptureKeyboard && ImGui::IsAnyItemFocused()) {

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
		//Check if editor is active
		if (!b_editor_active)
			return;

		//Get input to set
		ImGuiIO& io = ImGui::GetIO();

		//Check if imgui wants to capture mouse events
		if (io.WantCaptureMouse) {

			//Process key event
			switch (event->state) {
			case Input::States::PRESS: {
				io.MouseDown[mapCodeToIMGUICode(event->code)] = true;
				io.MouseClicked[mapCodeToIMGUICode(event->code)] = true;
				break;
			}
			case Input::States::REPEAT: {
				io.MouseDown[mapCodeToIMGUICode(event->code)] = true;
				break;
			}
			case Input::States::RELEASE: {
				io.MouseDown[mapCodeToIMGUICode(event->code)] = false;
				io.MouseClicked[mapCodeToIMGUICode(event->code)] = false;
				break;
			}
			default: {
				break;
			}
			}

			//Game game panel functions
			auto game_panel = std::dynamic_pointer_cast<GameWindowPanel>(panels_map.at(GameWindowPanel::getStaticName()));

			//Check if current mouse pos is within game window
			if (!game_panel->isMouseInWindow() || game_panel->checkPopUpShowing()) {
				event->setEventProcessed(true);
			}
		}
	}

	void LevelEditor::Service::onEvent(std::shared_ptr<Input::MouseMovedEvent> event) {
		//Check if editor is active
		if (!b_editor_active)
			return;

		//Get input to set
		ImGuiIO& io = ImGui::GetIO();

		//Check if imgui wants to capture mouse events
		if (io.WantCaptureMouse) {

			//Update imgui mouse position
			io.MousePos.x = event->window_pos.x;
			io.MousePos.y = event->window_pos.y;

			//Game game panel functions
			auto game_panel = std::dynamic_pointer_cast<GameWindowPanel>(panels_map.at(GameWindowPanel::getStaticName()));

			//Check if current mouse pos is within game window
			if (game_panel->isMouseInWindow() && !game_panel->checkPopUpShowing()) {
				event->world_pos = game_panel->getWorldMousePos();
				event->window_pos = game_panel->getWindowMousePos();
			}
			else {
				//Mark event as processed
				event->setEventProcessed(true);
			}
		}
	}

	void LevelEditor::Service::onEvent(std::shared_ptr<Input::MouseScrollEvent> event) {
		//Check if editor is active
		if (!b_editor_active)
			return;

		//Get input to set
		ImGuiIO& io = ImGui::GetIO();

		//Check if imgui wants to capture mouse events
		if (io.WantCaptureMouse) {

			//Update scroll offset
			io.MouseWheelH = static_cast<float>(event->offset.x);
			io.MouseWheel = static_cast<float>(event->offset.y);

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

	void LevelEditor::Service::updateShortCuts() {
		ImGuiIO& io = ImGui::GetIO();
		auto game_panel = std::dynamic_pointer_cast<GameWindowPanel>(panels_map.at(GameWindowPanel::getStaticName()));

		//Toggle Level Editor On & Off ( Use Global NIKE input to toggle on and off )
		if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_TAB) && game_panel->isMouseInWindow()) {

			//Toggle editor mode
			b_editor_active = !b_editor_active;

			//Set game to play
			if (!b_editor_active) {

				//Save editor data
				autoSave();
				
				//Play game
				std::dynamic_pointer_cast<MainPanel>(panels_map.at(MainPanel::getStaticName()))->setGameState(true);

				//Restart scene
				NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::RESTART, ""));
			}
			else {

				//Pause the game
				std::dynamic_pointer_cast<MainPanel>(panels_map.at(MainPanel::getStaticName()))->setGameState(false);

				//Restart scene
				NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::RESTART, ""));
			}

			// Clear all inputs done while editor is not active
			io.ClearEventsQueue();
		}

		//Return if editor is not active
		if (!b_editor_active)
			return;

		//Undo
		static bool z_triggered = false;
		if (io.KeyCtrl && io.KeysDown[ImGuiKey_Z]) {
			if (!z_triggered) {
				action_manager->undo();
				z_triggered = true;
			}
		}
		else {
			z_triggered = false;
		}

		//Redo
		static bool y_triggered = false;
		if (io.KeyCtrl && io.KeysDown[ImGuiKey_Y]) {
			if (!y_triggered) {
				action_manager->redo();
				y_triggered = true;
			}
		}
		else {
			y_triggered = false;
		}
	}

	void LevelEditor::Service::init() {

		//Init Imgui
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

		// Load imgui settings
		ImGui::LoadIniSettingsFromDisk(io.IniFilename);

		//Init editor action manager
		action_manager = std::make_unique<ActionManager>();

		//Add main panel
		auto main_panel = std::make_shared<MainPanel>();
		panels.push_back(main_panel);
		panels_map.emplace(main_panel->getName(), main_panel);

		//Add entities management panel
		auto entities_panel = std::make_shared<EntitiesPanel>();
		panels.push_back(entities_panel);
		panels_map.emplace(entities_panel->getName(), entities_panel);

		//Add components management panel
		auto components_panel = std::make_shared<ComponentsPanel>();
		panels.push_back(components_panel);
		panels_map.emplace(components_panel->getName(), components_panel);

		//Add prefab management panel
		auto prefab_panel = std::make_shared<PrefabsPanel>();
		panels.push_back(prefab_panel);
		panels_map.emplace(prefab_panel->getName(), prefab_panel);

		//Add debug management panel
		auto debug_panel = std::make_shared<DebugPanel>();
		panels.push_back(debug_panel);
		panels_map.emplace(debug_panel->getName(), debug_panel);

		//Add audio management panel
		auto audio_panel = std::make_shared<AudioPanel>();
		panels.push_back(audio_panel);
		panels_map.emplace(audio_panel->getName(), audio_panel);

		//Add resource management panel
		auto resource_panel = std::make_shared<ResourcePanel>();
		panels.push_back(resource_panel);
		panels_map.emplace(resource_panel->getName(), resource_panel);

		//Add camera management panel
		auto camera_panel = std::make_shared<CameraPanel>();
		panels.push_back(camera_panel);
		panels_map.emplace(camera_panel->getName(), camera_panel);

		//Add UI management panel
		auto ui_panel = std::make_shared<UIPanel>();
		panels.push_back(ui_panel);
		panels_map.emplace(ui_panel->getName(), ui_panel);

		//Add Tile Map management panel
		auto tile_map_panel = std::make_shared<TileMapPanel>();
		panels.push_back(tile_map_panel);
		panels_map.emplace(tile_map_panel->getName(), tile_map_panel);

		//Add game window panel
		auto game_panel = std::make_shared<GameWindowPanel>();
		panels.push_back(game_panel);
		panels_map.emplace(game_panel->getName(), game_panel);

		// Add scenes management window panel
		auto scenes_panel = std::make_shared<ScenesPanel>();
		panels.push_back(scenes_panel);
		panels_map.emplace(scenes_panel->getName(), scenes_panel);

		//Init all level editor panels
		std::for_each(panels.begin(), panels.end(), [](std::shared_ptr<IPanel> panel) { panel->init(); });
	}

	void LevelEditor::Service::updateAndRender() {

		//Update imgui with proper delta time
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = NIKE_WINDOWS_SERVICE->getDeltaTime();

		//Update shortcuts
		updateShortCuts();

		//Return when imgui is not active
		if (!b_editor_active) {
			return;
		}

		//Begin render frame
		beginFrame();

		//Render all panels
		for (auto& panel : panels) {
			panel->render();
		}

		//End render frame
		endFrame();

		//Save imgui layouts
		ImGui::SaveIniSettingsToDisk(io.IniFilename);
	}

	void LevelEditor::Service::cleanUp() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void LevelEditor::Service::autoSave() const {

		//Main panel
		auto main_panel = std::dynamic_pointer_cast<MainPanel>(panels_map.at(MainPanel::getStaticName()));

		//Check if auto save is on
		if (main_panel->getAutoSave()) {

			//Check if main panel game is active
			if (!main_panel->getGameState()) {

				//Save scene if game is not playing
				std::dynamic_pointer_cast<ScenesPanel>(panels_map.at(ScenesPanel::getStaticName()))->saveScene();
			}

			//Save prefab
			std::dynamic_pointer_cast<PrefabsPanel>(panels_map.at(PrefabsPanel::getStaticName()))->savePrefab();
		}
	}

	void LevelEditor::Service::setEditorState(bool state) {
		b_editor_active = state;
	}

	bool LevelEditor::Service::getEditorState() const {
		return b_editor_active;
	}

	bool LevelEditor::Service::getDebugState() const {
		return std::dynamic_pointer_cast<MainPanel>(panels_map.at(MainPanel::getStaticName()))->getDebugState();
	}

	bool LevelEditor::Service::getGameState() const {
		return std::dynamic_pointer_cast<MainPanel>(panels_map.at(MainPanel::getStaticName()))->getGameState();
	}

	void LevelEditor::Service::bindEditorFrameBuffer() const {
		//render to framebuffer if imgui is active
		if (b_editor_active) {
			NIKE_RENDER_SERVICE->bindFrameBuffer(std::dynamic_pointer_cast<GameWindowPanel>(panels_map.at(GameWindowPanel::getStaticName()))->getEditorFrameBuffer());
		}
		else {
			NIKE_RENDER_SERVICE->unbindFrameBuffer();
		}
	}

	void LevelEditor::Service::unbindEditorFrameBuffer() const {
		if (b_editor_active) {
			NIKE_RENDER_SERVICE->unbindFrameBuffer();
		}
	}

	void LevelEditor::Service::addPanel(std::shared_ptr<LevelEditor::IPanel> panel) {
		auto it = panels_map.find(panel->getName());
		if (it != panels_map.end()) {
			throw std::runtime_error("Panel already added.");
		}

		panels.push_back(panel);
		panels_map.emplace(panel->getName(), panel);
	}

	void LevelEditor::Service::removePanel(std::string const& panel_id) {
		auto it = panels_map.find(panel_id);
		if (it == panels_map.end()) {
			throw std::runtime_error("Panel doest not exist.");
		}

		//Remove panel from panel vector
		for (int i = 0; i < panels.size(); i++) {
			if (panels.at(i) == it->second) {
				panels.erase(panels.begin());
				break;
			}
		}
		//Remove from map
		panels_map.erase(it);
	}

	std::shared_ptr<LevelEditor::IPanel> LevelEditor::Service::getPanel(std::string const& panel_id) {
		auto it = panels_map.find(panel_id);
		if (it == panels_map.end()) {
			throw std::runtime_error("Panel doest not exist.");
		}

		return panels_map.at(panel_id);
	}

	void LevelEditor::Service::executeAction(Action&& action) {
		action_manager->executeAction(std::move(action));
	}

	std::unordered_map<std::string, std::function<void(LevelEditor::ComponentsPanel&, void*)>>& LevelEditor::Service::getComponentsUI() {
		return std::dynamic_pointer_cast<LevelEditor::ComponentsPanel> (panels_map.at(LevelEditor::ComponentsPanel::getStaticName()))->getCompsUI();
	}

	void LevelEditor::Service::deserializeConfig(nlohmann::json const& config) {
		try {
			// Set editor active state
			b_editor_active = config.at("EditorConfig").at("Editor_State").get<bool>();

			// Deserialize editor config
			std::dynamic_pointer_cast<MainPanel>(panels_map.at(MainPanel::getStaticName()))->deserializeConfig(config);

		}
		catch (const std::exception& e) {
			NIKEE_CORE_WARN(e.what());
			NIKEE_CORE_WARN("Error during config deserialization");
		}
	}
}
