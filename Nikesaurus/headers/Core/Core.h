#pragma once

#ifdef NIKESAURUS_PLATFORM_WINDOWS
	#ifdef NIKE_BUILD_DLL
		#define NIKESAURUS_API __declspec(dllexport)
	#else
		#define NIKESAURUS_API __declspec(dllimport)
	#endif
#else
#endif