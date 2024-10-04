/*****************************************************************//**
 * \file   uClock.cpp
 * \brief	Clock object class
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Utility/uSystems.h"

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