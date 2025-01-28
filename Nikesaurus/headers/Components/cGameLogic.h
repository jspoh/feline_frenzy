/*****************************************************************//**
 * \file   cGameLogic.h
 * \brief  Game Logic components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef C_GAMELOGIC_HPP
#define C_GAMELOGIC_HPP

namespace NIKE {
	namespace GameLogic {

		struct State {
			std::function<void()> onEnter;     // Lua function for enter action
			std::function<void()> onExit;      // Lua function for exit action
			std::unordered_map<std::string, std::string> transitions; // map of transition names to destination states
		};

		struct StateMachine {
			std::string currentState;
			std::unordered_map<std::string, State> states;
			std::unordered_map<std::string, std::function<bool()>> transitions; // Use Lua functions as conditions
		};

		//Movement logic component
		struct ILogic {
			Lua::Script script;

			ILogic() : script() {}
			// Movement(std::string const& script) : script(script) {}
		};

		void registerComponents();
	}
}

#endif //!C_GAMELOGIC_HPP