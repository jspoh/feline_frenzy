/*****************************************************************//**
 * \file   stdafx.h
 * \brief  Precompiled header file for standard application framework extensions
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   11 September 2024
 *********************************************************************/

#pragma once


#define _CRTDBG_MAP_ALLOC

// graphics headers
#include "GL/glew.h"
#include "GLFW/glfw3.h"

// Fmod headers
#include "FMOD/fmod.h"
#include "FMOD/fmod_codec.h"
#include "FMOD/fmod_common.h"
#include "FMOD/fmod_dsp.h"
#include "FMOD/fmod_dsp_effects.h"
#include "FMOD/fmod_errors.h"
#include "FMOD/fmod_output.h"

// standard headers
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

// external headers
#include "json.hpp"

// our completed headers
#include "Vector2.h"
#include "Vector4.h"

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
