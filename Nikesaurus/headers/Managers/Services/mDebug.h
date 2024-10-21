/*****************************************************************//**
 * \file   mDebug.h
 * \brief  Debug manager function declarations
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef DEBUG_MANAGER_HPP
#define DEBUG_MANAGER_HPP

#include "Managers/ECS/mSystem.h"

namespace NIKESAURUS {
	namespace Debug {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		class NIKESAURUS_API Manager
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

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif