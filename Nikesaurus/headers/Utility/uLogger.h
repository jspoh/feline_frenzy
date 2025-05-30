/*****************************************************************//**
 * \file   uLogger.h
 * \brief  logging function declarations
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

namespace NIKE {

	//Temporary Disable DLL Export Warning
	#pragma warning(disable: 4251)

	class NIKE_API Log
	{
	public:

		static void Init();

		// Logging
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		
		inline static std::shared_ptr<spdlog::logger>& GetCrashFileLogger() { return s_CrashFileLogger; }

		static void InitCrashLogger();

	private:

		// Logging 
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		static std::shared_ptr<spdlog::logger> s_CrashFileLogger;

		// Debugging
		static char documents_path[MAX_PATH];
	};

	//Re-enable DLL Export warning
	#pragma warning(default: 4251)

}

// CORE LOGGING MACROS (USED FOR ERROR DEBUGGING IN ENGINE)
#define NIKEE_CORE_TRACE(...)	NIKE::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NIKEE_CORE_INFO(...)	NIKE::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NIKEE_CORE_WARN(...)	NIKE::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NIKEE_CORE_ERROR(...)	NIKE::Log::GetCoreLogger()->error(__VA_ARGS__)


// CLIENT LOGGING MACROS (USED FOR ERROR DEBUGGING IN GAME)
#define NIKEE_TRACE(...)		NIKE::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NIKEE_INFO(...)			NIKE::Log::GetClientLogger()->info(__VA_ARGS__)
#define NIKEE_WARN(...)			NIKE::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NIKEE_ERROR(...)		NIKE::Log::GetClientLogger()->error(__VA_ARGS__)

// CRASH LOGGER (USED FOR EXCEPTION ERRORS)
#define LOG_CRASH(message) \
    do { \
        if (!NIKE::Log::GetCrashFileLogger()) { \
            NIKE::Log::InitCrashLogger(); \
        } \
        NIKE::Log::GetCrashFileLogger()->error("{} (crash occurred in file: {} line: {} in function {}())", message, __FILE__, __LINE__, __func__); \
        if (IsDebuggerPresent()) { \
            __debugbreak(); \
        } \
        if (NIKE_WINDOWS_SERVICE && NIKE_WINDOWS_SERVICE->getWindow()) { \
            NIKE_WINDOWS_SERVICE->getWindow()->cleanUp(); \
        } \
        assert(false && message); \
    } while (0)
