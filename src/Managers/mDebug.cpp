/*****************************************************************//**
 * \file   mDebug.cpp
 * \brief  Debug manager function definitions
 *
 * \author Bryan Lim
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
}

