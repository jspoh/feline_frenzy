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

			//Undo Redo Manager
			std::unique_ptr<ActionManager> action_manager;

			//Vector of editor panels
			std::vector<std::shared_ptr<IPanel>> panels;

			//Map of editor panels
			std::unordered_map<std::string, std::shared_ptr<IPanel>> panels_map;

			//Main panel
			std::shared_ptr<MainPanel> main_panel;

			//Game panel
			std::shared_ptr<GameWindowPanel> game_panel;
		public:
			Service() : b_editor_active{ false } {}
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


			//Get Debug State
			bool getDebugState() const;

			//Get Game State
			bool getGameState() const;

			//Add panel ( added into map based on IPanel getName() )
			void addPanel(std::shared_ptr<IPanel> panel);

			//Remove panel ( retrieved into map based on IPanel getName() )
			void removePanel(const std::string& panel_id);

			//Execute action made
			void executeAction(Action const& action);
		};
	}
}

#endif //!LEVEL_EDITOR_HPP