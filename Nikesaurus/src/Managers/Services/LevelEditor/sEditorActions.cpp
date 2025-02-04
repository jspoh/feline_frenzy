/*****************************************************************//**
 * \file   sEditorActions.cpp
 * \brief  Level editor undo & redo functionality
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/LevelEditor/sEditorActions.h"

namespace NIKE {
	void LevelEditor::ActionManager::undo() {

		//Check if undo stack is empty
		if (undo_stack.empty()) {
			NIKEE_CORE_WARN("End Of Undo Stack.");
			return;
		}

		//Execute the undo action at the top of undo stack
		undo_stack.top().undo_action();

		//Push undone action into the redo stack
		redo_stack.push(std::move(undo_stack.top()));

		//Remove action from undo stack
		undo_stack.pop();
	}

	void LevelEditor::ActionManager::redo() {

		//Check if redo stack is empty
		if (redo_stack.empty()) {
			NIKEE_CORE_WARN("End Of Redo Stack.");
			return;
		}

		//Execute the redo action at the top of redo stack
		redo_stack.top().do_action();

		//Push done action into the undo stack
		undo_stack.push(std::move(redo_stack.top()));

		//Remove action from undo stack
		redo_stack.pop();
	}

	void LevelEditor::ActionManager::executeAction(Action&& action) {

		//Execute action immediatedly
		action.do_action();

		//Push executed action onto the undo action stack
		undo_stack.push(std::move(action));

		//Clear redo stack ( Resetted upon new action )
		while (!redo_stack.empty()) {
			redo_stack.pop();
		}
	}

	void LevelEditor::ActionManager::clearStacks() {
		//Clear redo stack
		while (!redo_stack.empty()) {
			redo_stack.pop();
		}

		//Clear undo stack
		while (!undo_stack.empty()) {
			undo_stack.pop();
		}
	}
}