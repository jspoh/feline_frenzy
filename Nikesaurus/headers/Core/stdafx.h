/*****************************************************************//**
 * \file   stdafx.h
 * \brief  Precompiled header file for standard application framework extensions
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   11 September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

 /*****************************************************************//**
 * Engine Specific Library
 *********************************************************************/
#ifdef NIKE_BUILD_DLL

// graphics headers
#include "GL/glew.h"
#include "GLFW/glfw3.h"

// Imgui headers
#include "ImGui/headers/imgui.h"
#include "ImGui/headers/imgui_impl_opengl3.h"
#include "ImGui/headers/imgui_impl_glfw.h"

#undef APIENTRY

 /*****************************************************************//**
 * FMOD Core
 *********************************************************************/
#include "FMOD_CORE/fmod.h"
#include "FMOD_CORE/fmod.hpp"
#include "FMOD_CORE/fmod_codec.h"
#include "FMOD_CORE/fmod_common.h"
#include "FMOD_CORE/fmod_dsp.h"
#include "FMOD_CORE/fmod_dsp_effects.h"
#include "FMOD_CORE/fmod_errors.h"
#include "FMOD_CORE/fmod_output.h"

 /*****************************************************************//**
 * Freetype
 *********************************************************************/
#include "freetype/freetype.h"
#include "ft2build.h"

#endif //!!!Nest Engine specific includes within

//Memory
#define _CRTDBG_MAP_ALLOC

 /*****************************************************************//**
 * Windows Application
 *********************************************************************/
#ifdef NIKE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

 /*****************************************************************//**
 * NIKE CORE HEADER
 *********************************************************************/
#include "Core.h"

 /*****************************************************************//**
 * Spdlog (LOGGING)
 *********************************************************************/
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

 /*****************************************************************//**
 * STL
 *********************************************************************/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <cctype>
#include <sstream>
#include <time.h>
#include <list>
#include <sstream>
#include <memory>
#include <cmath>
#include <iterator>
#include <queue>
#include <set>
#include <unordered_set>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <thread>
#include <optional>

 /*****************************************************************//**
 * Data (json)
 *********************************************************************/
#include "Data/json.hpp"

 /*****************************************************************//**
 * Math headers
 *********************************************************************/
#include "Math/Mtx33.h"	
#include "Math/Vector2/Vector2.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Vector4/Vector4.h"

//Using math namespace
using namespace NIKE::Math;

/*****************************************************************//**
* Utility
*********************************************************************/
#include "Utility/uSystems.h"
#include "Utility/uLogger.h"

/*****************************************************************//**
* Math Defines
*********************************************************************/
#include <corecrt_math_defines.h>

/*****************************************************************//**
* Nullstream for release
*********************************************************************/
class NullStream {
public:
    NullStream() = default;
    ~NullStream() = default;

    template <typename T>
    NullStream& operator<<([[maybe_unused]] T _) {
        return *this;
    }
};
extern NullStream nullstream;

#ifndef NDEBUG
    #define cout std::cout
    #define endl std::endl
    #define cerr std::cerr
#else
    #define cout nullstream
    #define cerr nullstream
    #define endl ""
#endif
