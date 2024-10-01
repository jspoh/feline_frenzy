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

namespace Debug
{
	class Manager
	{
	public:
		Manager() = default;
		~Manager() = default;
		void logCrash();

	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

	};
}



#endif