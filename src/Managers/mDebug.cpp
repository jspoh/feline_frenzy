/*****************************************************************//**
 * \file   mDebug.cpp
 * \brief  Debug manager function definitions
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mDebug.h"

namespace Debug
{
	void Manager::logCrash()
	{
		std::ofstream log_file("src/Managers/crash_log.txt", std::ios::app);
		if (!log_file.is_open())
		{
			cout << "file not found" << endl;
		}
		// Get current log crash time
		std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
		// Convert to time_t for printing as calendar time
		std::time_t crash_time = std::chrono::system_clock::to_time_t(current_time);

		struct tm time_info;
		localtime_s(&time_info, &crash_time);

		log_file << "Crash occurred at: " << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S") << endl;

		// Close after logging
		log_file.close();
	}

	void Manager::systemRuntimePercentage(double game_loop_time, std::vector<double> const& system_times, std::vector<std::shared_ptr<System::ISystem>> systems)
	{
		if (system_times.empty()) {
			cout << "No active systems to report on." << endl;
			return;
		}

		// Calculate total time for all systems
		double total_system_time = 0.0;

		// Print header for system performance percentages
		cout << "System Performance (% of total game loop time):" << endl;

		// Calculate the time spent in each active system
		for (size_t i = 0; i < systems.size(); ++i)
		{
			if (systems[i]->getActiveState())
			{
				// Calculate the percentage of the total time for each active system
				double percentage = (system_times[i] / game_loop_time) * 100.0;
				cout << systems[i]->getSysName() << ": " << percentage << "%" << endl;

				// Accumulate total system time
				total_system_time += percentage;
			}
		}

		// Print total system percentage
		cout << "Total Active System Time: " << total_system_time << "%" << endl;

	}
}

