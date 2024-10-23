#pragma once

namespace NIKESAURUS {

	//Temporary Disable DLL Export Warning
	#pragma warning(disable: 4251)

	class NIKESAURUS_API Log
	{
	public:

		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:

		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;


	};

	//Re-enable DLL Export warning
	#pragma warning(default: 4251)

}

// CORE LOGGING MACROS
#define NIKEE_CORE_TRACE(...)	NIKESAURUS::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NIKEE_CORE_INFO(...)	NIKESAURUS::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NIKEE_CORE_WARN(...)	NIKESAURUS::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NIKEE_CORE_ERROR(...)	NIKESAURUS::Log::GetCoreLogger()->error(__VA_ARGS__)
#define NIKEE_CORE_FATAL(...)	NIKESAURUS::Log::GetCoreLogger()->fatal(__VA_ARGS__)


// CLIENT LOGGING MACROS
#define NIKEE_TRACE(...)		NIKESAURUS::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NIKEE_INFO(...)			NIKESAURUS::Log::GetClientLogger()->info(__VA_ARGS__)
#define NIKEE_WARN(...)			NIKESAURUS::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NIKEE_ERROR(...)		NIKESAURUS::Log::GetClientLogger()->error(__VA_ARGS__)
#define NIKEE_FATAL(...)		NIKESAURUS::Log::GetClientLogger()->fatal(__VA_ARGS__)