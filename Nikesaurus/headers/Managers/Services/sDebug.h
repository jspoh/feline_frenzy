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

namespace NIKE {
	namespace Debug {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		class NIKE_API Service
		{
		public:
			Service() : total_system_time{ 0.0 }{}
			~Service() = default;

			// Check for system runtime 
			void updateSystemPercentage(std::vector<double> const& sys_time, std::vector<std::shared_ptr<System::ISystem>> sys);

			//Get system percentages
			std::vector<std::pair<std::string, double>> getSystemPercentages() const;

			//Get total system time
			double getTotalSystemTime() const;
		private:

			std::vector<std::pair<std::string, double>> system_percentages;
			double total_system_time;

			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif