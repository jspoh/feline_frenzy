/*****************************************************************//**
 * \file   cMove.h
 * \brief
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef C_MOVE_HPP
#define C_MOVE_HPP

#include "Core/stdafx.h"

namespace NIKESAURUS {
	namespace Move {
		struct NIKESAURUS_API Movement {
			bool Up;
			bool Down;
			bool Left;
			bool Right;
		};
	}
}

#endif // !C_MOVE_HPP
