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
                std::vector<NIKE::Math::Vector2f> path; // Path is a vector of waypoints
                //int current_index;   // Index of current target waypoint in path
                //bool path_found;   // Flag to indicate if path has been found

                Path() = default;
        };

        // Register the Pathfinding component
        void registerComponents();
    }
}
#endif // !C_PATHFINDING_HPP