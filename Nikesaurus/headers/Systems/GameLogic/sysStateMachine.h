/*****************************************************************//**
 * \file   sysStateMachine.h
 * \brief  state machine game logic system
 *
 * \author Ho
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once


namespace NIKE {
    namespace StateMachine {

        // Entity state machine types
        // Struct to represent state transitions in the state machine
        struct StateTransition {
            EntityStateType from;      // Starting state for the transition
            EntityStateType to;        // Target state after the transition
            std::function<bool()> condition;  // Condition for triggering the transition

            // Constructor to initialize transition with starting state, target state, and condition
            StateTransition(EntityStateType from_state, EntityStateType to_state, std::function<bool()> condition)
                : from(from_state), to(to_state), condition(std::move(condition)) {}
        };

        class Manager {
        public:
            // Initialize with a default state, entity type, and reset timer
            explicit Manager(EntityStateType initial_state, EntityLogicType entity_type)
                : current_state(initial_state), entity_type(entity_type), state_timer(0.0f) {}

            // Method to add a state transition to the machine
            void addTransition(EntityStateType from, EntityStateType to, std::function<bool()> condition) {
                transitions.emplace_back(from, to, std::move(condition));
            }

            // Update state machine by checking transition conditions and advancing timer
            void update(float dt) {
                // Check each transition for the current state
                for (const auto& transition : transitions) {
                    if (transition.from == current_state && transition.condition()) {
                        current_state = transition.to;  // Change to new state
                        state_timer = 0.0f;  // Reset timer for the new state
                        break;
                    }
                }
                state_timer += dt;  // Advance timer by delta time
            }

            // Accessor for state timer, used in conditions
            float getStateTimer() const { return state_timer; }

            // Accessor for current state
            EntityStateType getCurrentState() const { return current_state; }

        private:
            EntityStateType current_state;   // The current state of the entity
            EntityLogicType entity_type;     // The type of entity (used for differentiating logic)
            std::vector<StateTransition> transitions;  // List of possible transitions
            float state_timer;               // Timer to track time spent in the current state
        };
    }
}