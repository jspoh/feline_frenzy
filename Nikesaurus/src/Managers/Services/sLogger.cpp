/*****************************************************************//**
 * \file   .cpp
 * \brief  Debug manager function definitions
 *
 * \author , 2301326, @digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"

#include "Managers/Services/sLogger.h"

namespace NIKESAURUS {
	namespace Logging {
		std::shared_ptr<spdlog::logger> Service::s_CoreLogger;
		std::shared_ptr<spdlog::logger> Service::s_ClientLogger;

		void Service::Init()
		{
			spdlog::set_pattern("%^[%T] %n: %v%$");
			s_CoreLogger = spdlog::stderr_color_mt("NIKEEngine");
			s_CoreLogger->set_level(spdlog::level::trace);

			s_ClientLogger = spdlog::stderr_color_mt("Client");
			s_ClientLogger->set_level(spdlog::level::trace);
		}
		
	}
	
}