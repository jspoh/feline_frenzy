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
#include "Managers/Services/sMap.h"

namespace NIKE {
	namespace Pathfinding {
		struct Path {
			// Path to target
			// To seri only the cell's indexes
			std::deque<Map::Cell> path;
			Map::Cell start_cell;
			Map::Cell goal_cell;
			// Flag to indicate if path has been found
			bool b_finished;

			Path() = default;
		};

        // Register the Pathfinding component
        void registerComponents();
    }
}
#endif // !C_PATHFINDING_HPP