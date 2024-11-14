/*****************************************************************//**
 * \file   sEditorActions.cpp
 * \brief  Level editor undo & redo functionality
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/LevelEditor/sEditorActions.h"

namespace NIKE {
	void LevelEditor::ActionManager::undo() {

		//Check if undo stack is empty
		if (undo_stack.empty()) 
			return;

		//Execute the undo action at the top of undo stack
		LevelEditor::Action action = undo_stack.top();
		action.undo_action();

		//Remove action from undo stack
		undo_stack.pop();

		//Push undone action into the redo stack
		redo_stack.push(action);
	}

	void LevelEditor::ActionManager::redo() {

		//Check if redo stack is empty
		if (redo_stack.empty())
			return;

		//Execute the redo action at the top of redo stack
		LevelEditor::Action action = redo_stack.top();
		action.do_action();

		//Remove action from undo stack
		redo_stack.pop();

		//Push done action into the undo stack
		undo_stack.push(action);
	}

	void LevelEditor::ActionManager::executeAction(Action const& action) {

		//Execute action immediatedly
		action.do_action();

		//Push executed action onto the undo action stack
		undo_stack.push(action);

		//Clear redo stack ( Resetted upon new action )
		while (!redo_stack.empty()) {
			redo_stack.pop();
		}
	}
}