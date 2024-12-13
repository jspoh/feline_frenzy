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