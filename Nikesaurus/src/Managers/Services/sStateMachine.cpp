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
#include "Core/Engine.h"

// States
#include "Managers/States/enemyStates.h"

namespace NIKE {
	namespace StateMachine {
		void Service::changeState(std::shared_ptr<Istate> new_state)
		{

			if (!new_state) {
				throw std::invalid_argument("Attempting to change to invalid state!");
			}

			if (current_state == new_state) {
				// Safeguard for new state is the current state
				return;
			}

			if (current_state) {
				current_state->onExit();
			}

			// Change to the new state
			current_state = new_state;

			// Enter the new state
			current_state->onEnter();
		}

		void Service::registerState(const std::string& state_id, std::shared_ptr<Istate> state)
		{
			state_map[state_id] = std::move(state);
		}

		void Service::removeState(const std::string& state_id)
		{
			state_map.erase(state_id);
		}

		void Service::addTransition(const std::string& transition_id, std::shared_ptr<Itransition> transition)
		{
			transitions_map[transition_id] = transition;
		}

		std::shared_ptr<Istate> Service::getStateByID(const std::string& state_id) const
		{
			auto it = state_map.find(state_id);
			if (it != state_map.end()) {
				return it->second;
			}
			return nullptr;
		}

		void Service::init() {
			// Regiser states
			registerState("Idle", std::make_shared<State::IdleState>());
			registerState("Attack", std::make_shared<State::AttackState>());
			registerState("Chase", std::make_shared<State::ChaseState>());
		}

		void Service::update(Entity::Type& entity) {

			// A note to add here: entity is already passed in from the main for loop iteration from 
			// game logic system

			auto state_comp = NIKE_ECS_MANAGER->getEntityComponent<State::State>(entity);

			if (state_comp.has_value()) {
				// Lock the weak pointer to the current state
				current_state = state_comp.value().get().current_state.lock();
				if (current_state)
				{
					current_state->onUpdate();
				}
				else {
					NIKEE_CORE_WARN("State invalid");
				}

					for (auto& transition : transitions_map) {
					if (transition.second->isValid()) {
						std::shared_ptr<Istate> next_state = transition.second->getNextState();
						// Transition to the next state
						if (next_state)
						{
							changeState(next_state);
						}
					}
				}
			}


		}
	}
}
