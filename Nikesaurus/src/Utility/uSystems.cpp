/*****************************************************************//**
 * \file   uSystems.cpp
 * \brief	Utility systems
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Utility/uSystems.h"

namespace NIKE {
	Utility::Clock::Clock()
		: start{ std::chrono::high_resolution_clock::now() }, duration{ 0.0f }
	{
	}

	float Utility::Clock::getElapsedTime() {
		duration = std::chrono::high_resolution_clock::now() - start;
		return duration.count();
	}

	void Utility::Clock::restartClock() {
		start = std::chrono::high_resolution_clock::now();
	}

	float Utility::randFloat() {
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	std::string Utility::convertTypeString(std::string&& str_type) {
		return str_type.substr(str_type.find_first_not_of(':', str_type.find_first_of(':')), str_type.size() - str_type.find_first_not_of(':', str_type.find_first_of(':')));
	}
}