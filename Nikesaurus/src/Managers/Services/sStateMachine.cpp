/*****************************************************************//**
 * \file   sStateMachine.cpp
 * \brief  state machine service
 *
 * \author Bryan Lim Li Cheng
 * \date   January 2025
 * All content © 2025 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/sStateMachine.h"

namespace NIKE {
	namespace StateMachine {
		void Service::changeState(Istate* new_state)
		{
			if (current_state)
			{
				current_state->onExit();
				// Cleanup current state
				delete current_state;  
			}

			current_state = new_state;
			// Enter the new state
			current_state->onEnter();  
		}

		void Service::registerState(Istate* state)
		{
			state_list.push_back(state);
		}

		void Service::removeState(Istate* state)
		{
			state_list.erase(std::remove(state_list.begin(), state_list.end(), state), state_list.end());
		}

		void Service::initEntityFSM([[maybe_unused]] Entity::Type entity)
		{
			
		}	

		void Service::addTransition(Itransition* transition)
		{
			transition_list.push_back(transition);
		}

		void Service::init() {

		}

		void Service::update() {
			if (current_state) {
				// Update the current state
				current_state->onUpdate();  
			}

			for (auto& transition : transition_list) {
				if (transition->isValid()) {
					Istate* next_state = transition->getNextState();
					// Transition to the next state
					changeState(next_state);  
				}
			}
		}
	}
}
