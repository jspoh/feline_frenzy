/*****************************************************************//**
 * \file   StateManager.h
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#pragma once

#include "stdafx.h"
#include "State.h"


class StateManager {
private:
	std::unordered_map<std::string, State*> states;
	std::string active_state = "";

	StateManager() = default;
	~StateManager();		// safety net

public:
	static StateManager& getInstance();

	/**
	 * initializes all states
	 * to be called after StateManager is constructed
	 * 
	 */
	void register_all_states();

	/**
	 * Register state into program.
	 * 
	 * Steps:
	 * 1. overload State methods
	 * 2. register state - register_state("menu", state);
	 * 3. set active state - set_active_state("menu");
	 * 
	 * \param name
	 * \param state
	 */
	void register_state(const std::string& ref, State* state);

	/**
	 * if absolutely certain that the state will no longer be used,
	 * deregister the state to free up memory.
	 * 
	 * example: splash screen
	 * 
	 * \param ref
	 */
	void deregister_state(const std::string& ref);

	/**
	 * use this function to change the current state.
	 * 
	 * \param ref
	 */
	void set_active_state(const std::string& ref);

	/**
	 * logic to be used in update loop.
	 * 
	 */
	void run();
};
