/*****************************************************************//**
 * \file   sLevelEditor.h
 * \brief  Level editor
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef NDEBUG

#ifndef LEVEL_EDITOR_HPP
#define LEVEL_EDITOR_HPP

//Imgui Events
#include "../sEvents.h"
#include "../sInput.h"
#include "../sWindows.h"
#include "../Assets/sAssets.h"

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
		public:
			Service() : b_editor_active{ false } {}
			~Service() = default;

			//Init Editor
			void init();

			//Update Editor
			void updateAndRender();

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

			//Remove panel ( remove map based on IPanel getName() )
			void removePanel(std::string const& panel_id);

			//Get panel ( remove map based on IPanel getName() )
			std::shared_ptr<IPanel> getPanel(std::string const& panel_id);

			//Execute action made
			void executeAction(Action&& action);

			//Register UI component function
			template<typename T>
			void registerCompUIFunc(std::function<void(ComponentsPanel&, T&)> comp_func) {
				std::dynamic_pointer_cast<ComponentsPanel>(panels_map.at(ComponentsPanel::getStaticName()))->registerCompUIFunc<T>(comp_func);
			}

			// Deserialize editor config (required as level editor is init before systems are created)
			void deserializeConfig(nlohmann::json const& config);
		};
	}
}

#endif //!LEVEL_EDITOR_HPP


#endif
