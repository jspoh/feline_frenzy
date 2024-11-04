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

namespace NIKE {
	namespace Utility {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		/*****************************************************************//**
		* Clock object
		*********************************************************************/
		class NIKE_API Clock {
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
		float NIKE_API randFloat();

		//Convert type to string without nike::
		std::string convertTypeString(std::string&& str_type);

		// Utility functions
		template <typename T>
		T getMax(const T a, const T b) {
			return (((a) > (b)) ? (a) : (b));
		}

		template <typename T>
		T getMin(const T a, const T b) {
			return (((a) < (b)) ? (a) : (b));
		}

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif //!SYSTEM_UTILS_HPP