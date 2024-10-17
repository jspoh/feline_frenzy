/*****************************************************************//**
 * \file   uSystems.h
 * \brief	Utility systems
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef SYSTEM_UTILS_HPP
#define SYSTEM_UTILS_HPP

namespace Utility {

	/*****************************************************************//**
	* Clock object
	*********************************************************************/
	class Clock {
	private:
		//Start Time
		std::chrono::time_point<std::chrono::high_resolution_clock> start;

		//Current Duration
		std::chrono::duration<float> duration;

	public:
		//Default Constructor
		Clock();

		//Get Current Elapsed Time
		float getElapsedTime();

		//Restart Clock
		void restartClock();
	};

	//Generate random float
	float randFloat();
}

#endif //!SYSTEM_UTILS_HPP