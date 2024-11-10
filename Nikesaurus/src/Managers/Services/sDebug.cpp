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
		// DEPRECIATED
		//void Service::logCrash()
		//{
		//	std::ofstream log_file("src/Services/Services/crash_log.txt", std::ios::app);
		//	if (!log_file.is_open())
		//	{
		//		NIKEE_CORE_ERROR("file not found");
		//	}
		//	// Get current log crash time
		//	std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
		//	// Convert to time_t for printing as calendar time
		//	std::time_t crash_time = std::chrono::system_clock::to_time_t(current_time);

		//	struct tm time_info;
		//	localtime_s(&time_info, &crash_time);

		//	log_file << "Crash occurred at: " << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S") << endl;

		//	// Close after logging
		//	log_file.close();
		//}


		void Service::updateSystemPercentage(double gl_time, std::vector<double> const& sys_time, std::vector<std::shared_ptr<System::ISystem>> sys)
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
					double percentage = (sys_time[i] / gl_time) * 100.0;
					system_percentages.push_back({ sys[i]->getSysName(), percentage });

					// Accumulate total system time
					total_system_time += percentage;
				}
			}

		}
	}

}
