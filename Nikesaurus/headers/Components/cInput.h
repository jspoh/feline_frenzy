/*****************************************************************//**
 * \file   cInput.h
 * \brief
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef C_INPUT_HPP
#define C_INPUT_HPP

#include "Core/stdafx.h"

namespace NIKESAURUS {
	namespace Input {
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		//Input Mode
		enum class NIKESAURUS_API TriggerMode {
			CHECKED = 0,
			TRIGGERED,
			RELEASED
		};

		struct NIKESAURUS_API Key {
			//Input trigger mode
			TriggerMode mode;

			//Key type
			int key_type;

			//Output result
			bool b_output;
		};

		struct NIKESAURUS_API Mouse {
			//Input trigger mode
			TriggerMode mode{};

			//Pos of button
			Vector2 button_pos;

			//Button type triggered
			int button_type{};

			//Output result
			bool b_output = false;

			//most mousewheels will only scroll on y axis
			Vector2 scroll;
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif // !C_INPUT_HPP
