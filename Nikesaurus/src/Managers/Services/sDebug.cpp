/*****************************************************************//**
 * \file   mDebug.cpp
 * \brief  Debug manager function definitions
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/sDebug.h"

//Definition of nullstream for debugging
NullStream nullstream;

namespace NIKE {
	namespace Debug
	{
		void Service::updateSystemPercentage(std::vector<double> const& sys_time, std::vector<std::shared_ptr<System::ISystem>> sys)
		{
			if (sys_time.empty()) {
				NIKEE_CORE_INFO("No active systems to report on.");
				return;
			}

			// Calculate total time for all systems
			total_system_time = 0.0;
			system_percentages.clear();

			// Calculate the time spent in each active system
			for (size_t i = 0; i < sys.size(); ++i)
			{
				if (sys[i]->getActiveState())
				{
					// Calculate the percentage of the total time for each active system
					system_percentages.push_back({ sys[i]->getSysName(), sys_time[i] });

					// Accumulate total system time
					total_system_time += sys_time[i];
				}
			}

		}

		std::vector<std::pair<std::string, double>> Service::getSystemPercentages() const {
			return system_percentages;
		}

		double Service::getTotalSystemTime() const {
			return total_system_time;
		}
	}

}
