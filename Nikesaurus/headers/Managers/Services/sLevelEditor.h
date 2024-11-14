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

namespace NIKE {
	namespace LevelEditor {
		//Panel Interface
		class IPanel {
		private:
		public:
			virtual ~IPanel() = default;

			//Name
			virtual std::string getName() const = 0;

			//Init
			virtual void init() {};

			//Update
			virtual void update() = 0;

			//Render
			virtual void render() = 0;
		};

		//Service
		class Service {
		private:
			//Begin frame for Editor
			void beginFrame();

			//End frame for Editor
			void endFrame();

			//Boolean for activating editor
			bool b_editor_active;
			
			//Boolean for activating debug mode
			bool b_debug_mode;

			//Boolean for activating play/stop for game
			bool b_game_state;

			//Map of editor panels
			std::unordered_map<std::string, std::shared_ptr<IPanel>> panels;
		public:
			Service() = default;
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

			//Set Debug State
			void setDebugState(bool state);

			//Set Game State
			void setGameState(bool state);

			//Add panel
			void addPanel(const std::string& panelName, std::shared_ptr<IPanel> panel);

			//Remove panel
			void removePanel(const std::string& panelName);
		};
	}
}

#endif //!LEVEL_EDITOR_HPP