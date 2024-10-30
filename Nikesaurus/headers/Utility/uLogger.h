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

		// Debugging

	private:

		// Logging 
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		static std::shared_ptr<spdlog::logger> s_CrashFileLogger;

		// Debugging
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
        NIKE::Log::GetCrashFileLogger()->error("{} (crash occurred in file: {} line: {} in function {}())", message, __FILE__, __LINE__, __func__); \
		NIKE_EVENTS_SERVICE.getService<NIKE::Windows::Service>()->getWindow()->cleanUp(); \
		assert(false && message); \
    } while (0)