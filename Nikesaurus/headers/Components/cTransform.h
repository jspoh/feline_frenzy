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

#include "Core/stdafx.h"

namespace NIKE {
	namespace Transform {

		struct Transform {
			Vector2f position;
			Vector2f scale;
			float rotation;

			//Previous data variables for interpolation rendering
			Vector2f prev_position;
			Vector2f prev_scale;
			float prev_rotation;

			Transform() : position(0.f, 0.f), scale(1.f, 1.f), rotation{ 0.0f }, prev_position{ position }, prev_scale{ scale }, prev_rotation{ rotation } {}
			Transform(Vector2f const& position, Vector2f const& scale, float rotation)
				: position{ position }, scale{ scale }, rotation{ rotation }, prev_position{ position }, prev_scale{ scale }, prev_rotation{ rotation } {}
		};

		void registerComponents();
	}
}

#endif // !C_TRANSFORM_HPP