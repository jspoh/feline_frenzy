/*****************************************************************//**
 * \file   State.h
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#pragma once


class State {
public:
	State() = default;
	virtual ~State() = default;

	// flag to determine if resources have been loaded
	// if load has been called, this should be set to true
	// after unload is called, this should be set to false
	// !TODO: as of rn, not sure if required yet - depends on resource size
	bool resources_loaded = false;

	virtual void load() = 0;
	virtual void init() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void exit() = 0;
	virtual void unload() = 0;
};
