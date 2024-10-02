/*****************************************************************//**
 * \file   stdafx.h
 * \brief  Precompiled header file for standard application framework extensions
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   11 September 2024
 *********************************************************************/

#pragma once

#define _CRTDBG_MAP_ALLOC

 /*****************************************************************//**
 * Graphics
 *********************************************************************/
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#undef APIENTRY

 /*****************************************************************//**
 * Windows
 *********************************************************************/
#include <Windows.h>

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
#include <bitset>
#include <queue>
#include <set>
#include <ctime>
#include <iomanip>

 /*****************************************************************//**
 * Data (json)
 *********************************************************************/
#include "../headers/Data/json.hpp"

 /*****************************************************************//**
 * Math headers
 *********************************************************************/
#include "../headers/Math/Mtx33.h"	
#include "../headers/Math/Vector2.h"
#include "../headers/Math/Vector3.h"
#include "../headers/Math/Vector4.h"

 /*****************************************************************//**
 * Components
 *********************************************************************/
#include "../headers/Components/cRender.h"
#include "../headers/Components/cTransform.h"
#include "../headers/Components/cMove.h"

/*****************************************************************//**
* Utility
*********************************************************************/
#include "../headers/Utility/uSystems.h"

/*****************************************************************//**
* Math Defines
*********************************************************************/
#define _USE_MATH_DEFINES
#include <math.h>

/*****************************************************************//**
* Entity 
*********************************************************************/
namespace Entity {

    //Entity Type
    using Type = uint16_t;

    //Max number of entities to be created at a single point
    const Type MAX = 5000;
}

/*****************************************************************//**
* Component
*********************************************************************/
namespace Component {

    //Component Signature Type
    using Type = uint8_t;

    //Max components to be stored in a signature
    const Type MAX = 64;

    //Nested components signature
    using Signature = std::bitset<MAX>;
}

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
