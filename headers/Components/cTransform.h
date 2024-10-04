/*****************************************************************//**
 * \file   cTransform.h
 * \brief  
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (90%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (5%)
 * \co-author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (5%)
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef C_TRANSFORM_HPP
#define C_TRANSFORM_HPP

#include "../headers/Core/stdafx.h"

namespace Transform {

	struct Transform {
		Vector2 position;
		Vector2 scale;
		float rotation;
	};

	struct Runtime_Transform {
		bool runtime_scale_up = false;
		bool runtime_rotate = false;
		bool runtime_scale_down = false;
	};

	struct Velocity {
		Vector2 velocity;
	};
}

#endif // !C_TRANSFORM_HPP