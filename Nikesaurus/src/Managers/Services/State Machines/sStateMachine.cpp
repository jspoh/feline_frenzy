/*****************************************************************//**
 * \file   sStateMachine.cpp
 * \brief  state machine service
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 * All content � 2025 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/State Machine/sStateMachine.h"
#include "Core/Engine.h"

 // States
#include "Managers/Services/State Machine/enemyStates.h"
#include "Managers/Services/State Machine/destructableStates.h"

namespace NIKE {
	namespace StateMachine {

		std::unordered_map<std::string, std::shared_ptr<Itransition>> const& Istate::getTransitions()
		{
			return transitions;
		}

		void Istate::addTransition(const std::string& transition_id, std::shared_ptr<Itransition> transition)
		{
			transitions[transition_id] = std::move(transition);
		}

		void Istate::removeTransition(const std::string& transition_id)
		{
			transitions.erase(transition_id);
		}

		bool Istate::checkTransitionExist(const std::string& transition_id)
		{
			// Check if transition exist
			return (transitions.find(transition_id) != transitions.end());
		}

		void Istate::animationStart(Entity::Type& entity, int start_x, int start_y)
		{
			auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);
			if (e_animate_comp.has_value()) {
				static Vector2i prev_start = e_animate_comp.value().get().start_index;

				if (prev_start != Vector2i(start_x, start_y)) {
					e_animate_comp.value().get().start_index.x = start_x;
					e_animate_comp.value().get().start_index.y = start_y;
					prev_start = e_animate_comp.value().get().start_index;

					//Restart animation
					auto e_base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
					if (e_base_comp.has_value()) {
						e_base_comp.value().get().animation_mode = Animation::Mode::RESTART;
					}
				}
			}
		}

		void Istate::animationEnd(Entity::Type& entity, int end_x, int end_y)
		{
			auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);
			if (e_animate_comp.has_value()) {
				static Vector2i prev_end = e_animate_comp.value().get().end_index;

				if (prev_end != Vector2i(end_x, end_y)) {
					e_animate_comp.value().get().end_index.x = end_x;
					e_animate_comp.value().get().end_index.y = end_y;
					prev_end = e_animate_comp.value().get().end_index;

					//Restart animation
					auto e_base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
					if (e_base_comp.has_value()) {
						e_base_comp.value().get().animation_mode = Animation::Mode::RESTART;
					}
				}
			}
		}

		void Istate::flipX(Entity::Type& entity, bool yes_or_no)
		{
			auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
			if (e_texture_comp.has_value()) {
				if (e_texture_comp.value().get().b_flip.x != yes_or_no)
				{
					e_texture_comp.value().get().b_flip.x = yes_or_no;
				}
			}
		}

		void Istate::flipY(Entity::Type& entity, bool yes_or_no)
		{
			auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
			if (e_texture_comp.has_value()) {
				if (e_texture_comp.value().get().b_flip.y != yes_or_no)
				{
					e_texture_comp.value().get().b_flip.y = yes_or_no;
				}
			}
		}

		void Istate::setLastDirection(Entity::Type& entity, int dir)
		{
			auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
			if (e_physics_comp.has_value()) {
				e_physics_comp.value().get().last_direction = dir;
			}
		}

		int Istate::getLastDirection(Entity::Type& entity)
		{
			auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
			if (e_physics_comp.has_value()) {
				return e_physics_comp.value().get().last_direction;
			}
			// Return a rand value when cnt retrieve last dir
			return INT_MAX;
		}

		void Service::changeState(std::shared_ptr<Istate> new_state, Entity::Type& entity)
		{

			if (!new_state) {
				throw std::invalid_argument("Attempting to change to invalid state!");
			}

			if (current_state == new_state) {
				// Safeguard for new state is the current state
				return;
			}

			if (current_state) {
				current_state->onExit(entity);
			}

			// Change to the new state
			current_state = new_state;
			// Change entity's state component as well
			auto const& e_state_comp = NIKE_ECS_MANAGER->getEntityComponent<State::State>(entity);
			if (e_state_comp.has_value())
			{
				e_state_comp.value().get().current_state = new_state;
				// Change state id string as well
				e_state_comp.value().get().state_id = getStateID(new_state);
			}

			// Enter the new state
			current_state->onEnter(entity);
		}

		void Service::registerState(const std::string& state_id, std::shared_ptr<Istate> state)
		{
			state_map[state_id] = std::move(state);
		}

		void Service::removeState(const std::string& state_id)
		{
			state_map.erase(state_id);
		}

		std::string Service::getStateID(std::shared_ptr<Istate> state) const
		{
			for (const auto& elem : state_map)
			{
				if (elem.second == state)
				{
					return elem.first;
				}
			}

			// If state not found, return empty string
			return "";
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
			registerState("EnemyAttack", std::make_shared<State::EnemyAttackState>());
			registerState("EnemyChase", std::make_shared<State::EnemyChaseState>());
			registerState("EnemyHurt", std::make_shared<State::EnemyHurtState>());
			registerState("EnemyDeath", std::make_shared<State::EnemyDeathState>());
			registerState("DestructableDeath", std::make_shared<State::DestructableDeathState>());
		}

		void Service::update(Entity::Type& entity) {

			// A note to add here: entity is already passed in from the main for loop iteration from 
			// game logic system
			auto e_state_comp = NIKE_ECS_MANAGER->getEntityComponent<State::State>(entity);
			if (e_state_comp.has_value()) {
				// Assign entity ref to the comp
				auto& state_comp = e_state_comp.value().get();
				state_comp.entity_ref = entity;
				// Lock the weak pointer to the current state
				current_state = state_comp.current_state.lock();
				if (current_state)
				{
					current_state->onUpdate(entity);
					// Iterate current state's transitions
					for (auto& transition : current_state->getTransitions())
					{
						if (transition.second->isValid(entity)) {
							std::shared_ptr<Istate> next_state = transition.second->getNextState();
							// Transition to the next state
							if (next_state)
							{
								// Change state
								changeState(next_state, entity);
								// Break after changing state
								break;
							}
						}
					}
				}
				else {
					NIKEE_CORE_WARN("State invalid");
				}
			}
		}
	}
}
