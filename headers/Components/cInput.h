#pragma once

#ifndef C_INPUT_HPP
#define C_INPUT_HPP

#include "../headers/Core/stdafx.h"

namespace Input {
	//Input Mode
	enum class TriggerMode {
		CHECKED = 0,
		TRIGGERED,
		RELEASED
	};

	struct Key {
		//Input trigger mode
		TriggerMode mode;

		//Key type
		int key_type;

		//Output result
		bool b_output;
	};

	struct Mouse {
		//Input trigger mode
		TriggerMode mode;

		//Pos of button
		Vector2 button_pos;

		//Button type triggered
		int button_type;

		//Output result
		bool b_output;

		//most mousewheels will only scroll on y axis
		Vector2 scroll;
	};
}

#endif // !C_INPUT_HPP
