/*****************************************************************//**
 * \file   sEditorActions.h
 * \brief  Level editor undo & redo functionality
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef UNDO_REDO_EDITOR_HPP
#define UNDO_REDO_EDITOR_HPP

namespace NIKE {
	namespace LevelEditor {

		//Level editor actions
		struct Action {
			std::function<void()> do_action;
			std::function<void()> undo_action;
		};

		//Undo & Redo Level Editor Manager
		class ActionManager {
		private:

			//Undo stack actions
			std::stack<Action> undo_stack;

			//Redo stack actions
			std::stack<Action> redo_stack;
		public:
			ActionManager() = default;
			~ActionManager() = default;

			//Undo action
			void undo();

			//Redo Action
			void redo();

			//Excute action
			void executeAction(Action const& action);
		};

	}
}

#endif //!UNDO_REDO_EDITOR_HPP