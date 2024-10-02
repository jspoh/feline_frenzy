/*****************************************************************//**
 * \file   sysDebug.h
 * \brief  Debug manager function declarations
 *
 * \author Bryan Lim
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef DEBUG_MANAGER_HPP
#define DEBUG_MANAGER_HPP

#include "../headers/Managers/mSystem.h"

namespace Debug
{
	class Manager
	{
	public:
		Manager() = default;
		~Manager() = default;
		void logCrash();
		// Check for system runtime 
		void systemRuntimePercentage(double game_loop_time, std::vector<double> const& system_times, std::vector<std::shared_ptr<System::ISystem>> systems);

	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

	};
}



#endif