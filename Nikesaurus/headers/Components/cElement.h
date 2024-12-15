/*****************************************************************//**
 * \file   cElement.h
 * \brief  Defines the components used for Elements
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *********************************************************************/

#pragma once

#ifndef C_ELEMENT_HPP
#define C_ELEMENT_HPP

#include "Core/stdafx.h"

namespace NIKE {
	namespace Element {
		enum class Elements : int {
			NONE = 0,
			FIRE,
			WATER,
			GRASS,
		};

		constexpr float elemental_multiplier_table[4][4] = {
			//			NONE  FIRE   WATER  GRASS
			/* NONE */ { 1.0f, 1.0f, 1.0f, 1.0f },
			/* FIRE */ { 1.0f, 1.0f, 0.5f, 2.0f },
			/* WATER */ { 1.0f, 2.0f, 1.0f, 0.5f },
			/* GRASS */ { 1.0f, 0.5f, 2.0f, 1.0f }
		};


		struct Entity {
			Elements element;			// Currently equipped element
			// !TODO: Maybe make it random at the start?

			Entity() : element(Elements::NONE) {};
			Entity(int element)
				: element{ element } {}
		};

		struct Source {
			Elements element;			// Currently equipped element

			Source() : element(Elements::NONE) {};
			Source(int element)
				: element{ element } {}
		};

		void registerComponents();
	}
}


#endif // !C_ELEMENT_HPP