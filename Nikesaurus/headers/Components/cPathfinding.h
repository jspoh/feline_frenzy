/*****************************************************************//**
 * \file   cPathfinding.h
 * \brief
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef C_PATHFINDING_HPP
#define C_PATHFINDING_HPP

#include "Core/stdafx.h"
#include "Managers/Services/sEvents.h"

namespace NIKE {
	namespace Pathfinding {
        struct Path{
                std::vector<NIKE::Math::Vector2f> path; // Path as a vector of waypoints
                int currentIndex; // Index of the current target waypoint in path
                bool pathFound;   // Flag to indicate if path has been found

                Path() : currentIndex(0), pathFound(false) {}
        };

        // Register the Pathfinding component
        void registerComponents();
    }
}
#endif // !C_PATHFINDING_HPP