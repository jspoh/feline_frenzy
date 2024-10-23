#pragma once

namespace NIKESAURUS {
	namespace Logging {
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)
		class NIKESAURUS_API Service
		{
		public:
			Service() = default;
			~Service() = default;


			static void Init();

			inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
			inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

		private:
			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			static std::shared_ptr<spdlog::logger> s_CoreLogger;
			static std::shared_ptr<spdlog::logger> s_ClientLogger;


		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}

}

