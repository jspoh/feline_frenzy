/*****************************************************************//**
 * \file   MenuState.h
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#pragma once

#include "State.h"

class MenuState : public State {
private:
	MenuState();
	~MenuState();

public:
	static MenuState& getInstance();

	void load() override;
	void init() override;
	void update() override;
	void render() override;
	void exit() override;
	void unload() override;
};
