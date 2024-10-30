/*****************************************************************//**
 * \file   .cpp
 * \brief  Debug manager function definitions
 *
 * \author , 2301326, @digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"

#include "Utility/uLogger.h"

namespace NIKESAURUS {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
	std::shared_ptr<spdlog::logger> Log::s_CrashFileLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%X] %n: %v%$");

		// Core Logger (Logging related to engine)
		s_CoreLogger = spdlog::stderr_color_mt("NIKE_ENGINE");
		s_CoreLogger->set_level(spdlog::level::trace);
		
		// Client Logger (Logging related to game)
		s_ClientLogger = spdlog::stderr_color_mt("Feline Frenzy");
		s_ClientLogger->set_level(spdlog::level::trace);

		// Create a file sink (append crash log into file)
		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/crash-log.txt", false);

		// Crash Logger to file sink
		s_CrashFileLogger = std::make_shared<spdlog::logger>("Crash Log", file_sink);
		s_CrashFileLogger->set_pattern("[%D %X] %v");

		NIKEE_CORE_INFO("Logging Initialised");
	}
		
	
	
}