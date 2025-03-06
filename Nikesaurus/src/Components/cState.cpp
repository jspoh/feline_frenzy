/*****************************************************************//**
 * \file   cState.cpp
 * \brief  State Machine components
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu
 * \date   January 2025
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cState.h"

namespace NIKE {
	namespace State {

		State::State()
		{
			// Default init of entity's state to idle
			auto const& default_state = NIKE_FSM_SERVICE->getStateByID("Default");
			if (default_state)
			{
				current_state = default_state;
				state_id = "Default";
			}
		}

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
					UNREFERENCED_PARAMETER(data);
					// Default deserialize to idle
					comp.state_id = data.at("State").get<std::string>();
					// comp.state_id = "Idle";
					// Assign pointer based on state_id
					comp.current_state = NIKE_FSM_SERVICE->getStateByID(comp.state_id);

				},

				// Override Serialize
				[](State const& comp, State const& other_comp) -> nlohmann::json {
					nlohmann::json delta;

					if (comp.state_id != other_comp.state_id) {
						delta["State"] = comp.state_id;
					}

					return delta;
				},

				// Override Deserialize
				[](State& comp, nlohmann::json const& delta) {
					if (delta.contains("State")) {
						// comp.state_id = delta["State"].get<std::string>();
						comp.state_id = "Default";
						// Reassign the current state based on the new state_id
						comp.current_state = NIKE_FSM_SERVICE->getStateByID(comp.state_id);
					}
				}
			);

			NIKE_SERIALIZE_SERVICE->registerComponentAdding<State>();
	}

	void registerEditorComponents() {
#ifndef NDEBUG
	// UI Registration for StateComponent
	NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<State>(
		[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, State& comp) {

			// To display current state only
			{
				ImGui::Text("Current State:");
				ImGui::Text("%s", comp.state_id.c_str());
			}
		}
	);
#endif
		}

	}

}

