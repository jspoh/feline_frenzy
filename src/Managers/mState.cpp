/*****************************************************************//**
 * \file   StateManager.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mState.h"

// all possible states for initialization (registration)
#include "../headers/States/MenuState.h"

void StateManager::register_all_states() {
	// register all states here
	MenuState::getInstance();
}

StateManager::~StateManager() {
	for (std::pair<const std::string, State*>& state : states) {
		state.second->unload();
	}
	states.clear();
}

StateManager& StateManager::getInstance() {
	static StateManager instance;
	return instance;
}

void StateManager::register_state(const std::string& ref, State* state) {
	states[ref] = std::move(state);
}

void StateManager::deregister_state(const std::string& ref) {
	if (states.find(ref) == states.end()) {
		cerr << "Attempted to deregister state that does not exist" << endl;
		throw std::exception();
	}
	states[ref]->unload();
	delete states[ref];
	states.erase(ref);
}

void StateManager::set_active_state(const std::string& ref) {
	if (states.find(ref) == states.end()) {
		cerr << "Attempted to set active state that does not exist" << endl;
		throw std::exception();
	}
	if (active_state != "") {
		states[active_state]->exit();
		// wont unload resources on state change to avoid loading times
		// change only if we use too much memory (unlikely)
		//states[active_state]->unload();
	}
	active_state = ref;
	states[active_state]->load();
	states[active_state]->init();
}

void StateManager::run() {
	states[active_state]->update();
	states[active_state]->render();
}
