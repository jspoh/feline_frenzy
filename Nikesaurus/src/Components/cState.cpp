/*****************************************************************//**
 * \file   cFSM.cpp
 * \brief  State Machine components
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cState.h"

namespace NIKE {
	namespace State {
		void registerComponents()
		{
			// Register components
			NIKE_ECS_MANAGER->registerComponent<State>();

			// Register for Player serialization
			NIKE_SERIALIZE_SERVICE->registerComponent<State>(
				// Serialize
				[](State const& comp) -> nlohmann::json {
					return {
						{ "State" ,  comp.state_id}
					};
				},

				// Deserialize
				[](State& comp, nlohmann::json const& data) {
					comp.state_id = data.at("State").get<std::string>();

				}

			);


#ifndef NDEBUG

			// UI Registration for StateComponent
			NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<State>(
				[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, State& comp) {

					// For current state
					{
						ImGui::Text("Adjust State:");

						// List of available states
						static const char* state_names[] = { "Idle", "Chase", "Attack" };

						// Current and previous state tracking
						static std::string before_select_state = comp.state_id;
						int current_state_index = -1;

						// Find the current state index
						for (int i = 0; i < IM_ARRAYSIZE(state_names); ++i) {
							if (comp.state_id == state_names[i]) {
								current_state_index = i;
								break;
							}
						}

						// Handle combo box selection
						if (ImGui::Combo("##State", &current_state_index, state_names, IM_ARRAYSIZE(state_names))) {
							std::string new_state = state_names[current_state_index];
							if (new_state != comp.state_id) {
								// Save action
								LevelEditor::Action save_state;
								save_state.do_action = [&, state = new_state]() {
									comp.state_id = state;
									};

								// Undo action
								save_state.undo_action = [&, state = before_select_state]() {
									comp.state_id = state;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_state));

								// Update the previous value
								before_select_state = comp.state_id;

								// Apply the new state
								comp.state_id = new_state;
							}
						}
					}
				}
			);
		}
#endif
	}

}

