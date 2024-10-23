/*****************************************************************//**
 * \file   .cpp
 * \brief  Debug manager function definitions
 *
 * \author , 2301326, @digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"

#include "Utility/sLogger.h"

namespace NIKESAURUS {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%X] %n: %v%$");
		s_CoreLogger = spdlog::stderr_color_mt("NIKEEngine");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stderr_color_mt("Client");
		s_ClientLogger->set_level(spdlog::level::trace);

		NIKEE_CORE_INFO("Logging Initialised");
	}
		
	
	
}