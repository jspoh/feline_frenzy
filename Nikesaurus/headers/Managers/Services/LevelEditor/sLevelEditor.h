/*****************************************************************//**
 * \file   sLevelEditor.h
 * \brief  Level editor
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef LEVEL_EDITOR_HPP
#define LEVEL_EDITOR_HPP

//Imgui Events
#include "../sEvents.h"
#include "../sInput.h"
#include "../sWindows.h"

//Level Editor Helpers
#include "sEditorActions.h"
#include "sEditorPanels.h"

namespace NIKE {
	namespace LevelEditor {

		//Service
		class Service : 
			public Events::IEventListener<Input::KeyEvent>,
			public Events::IEventListener<Input::MouseBtnEvent>,
			public Events::IEventListener<Input::MouseMovedEvent>,
			public Events::IEventListener<Input::MouseScrollEvent>,
			public Events::IEventListener<Windows::WindowResized>
		{
		private:

			//Map NIKE To IMGUI Key & Button Codes
			int mapCodeToIMGUICode(int code);

			//On key btn event
			void onEvent(std::shared_ptr<Input::KeyEvent> event) override;

			//On mouse btn event
			void onEvent(std::shared_ptr<Input::MouseBtnEvent> event) override;

			//On mouse move event
			void onEvent(std::shared_ptr<Input::MouseMovedEvent> event) override;

			//On mouse scrolled event
			void onEvent(std::shared_ptr<Input::MouseScrollEvent> event) override;

			//On windows resized event
			void onEvent(std::shared_ptr<Windows::WindowResized> event) override;

			//Begin frame for Editor
			void beginFrame();

			//End frame for Editor
			void endFrame();

			//Update shortcuts
			void updateShortCuts();

			//Boolean for activating editor
			bool b_editor_active;
			
			//Boolean for activating debug mode
			bool b_debug_mode;

			//Boolean for activating play/stop for game
			bool b_game_state;

			//Undo Redo Manager
			std::unique_ptr<ActionManager> action_manager;

			//Map of editor panels
			std::unordered_map<std::string, std::shared_ptr<IPanel>> panels;

			//Game panel
			std::shared_ptr<GameWindowPanel> game_panel;
		public:
			Service() : b_editor_active{ false }, b_debug_mode{ false }, b_game_state{ true }{}
			~Service() = default;

			//Init Editor
			void init();

			//Update Editor
			void update();

			//Render Editor
			void render();

			//Clean up Editor
			void cleanUp();

			//Set Editor State
			void setEditorState(bool state);

			//Get Editor State
			bool getEditorState() const;

			//Set Debug State
			void setDebugState(bool state);

			//Get Debug State
			bool getDebugState() const;

			//Set Game State
			void setGameState(bool state);

			//Get Game State
			bool getGameState() const;

			//Add panel
			void addPanel(const std::string& panelName, std::shared_ptr<IPanel> panel);

			//Remove panel
			void removePanel(const std::string& panelName);

			//Execute action made
			void executeAction(Action const& action);
		};
	}
}

#endif //!LEVEL_EDITOR_HPP