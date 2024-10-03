/*****************************************************************//**
 * \file   cMove.h
 * \brief
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef C_MOVE_HPP
#define C_MOVE_HPP

#include "../headers/Core/stdafx.h"

namespace Move {
	struct Movement {
		bool Up;
		bool Down;
		bool Left;
		bool Right;
	};
}

#endif // !C_MOVE_HPP
