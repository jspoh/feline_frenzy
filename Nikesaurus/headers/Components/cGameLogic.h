/*****************************************************************//**
 * \file   cGameLogic.h
 * \brief	Game logic components
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

		//Script data
		//struct Script {
		//	std::string script_id;
		//	std::string script_path;
		//	std::string function;
		//	bool b_loaded;

		//	Script() : script_id{ "" }, script_path{ "" }, function{ "" }, b_loaded{ false } {}
		//	Script(std::string const& script) : script_id{ "" }, script_path{ script }, function{ "" }, b_loaded{ false } {}
		//};

		//Movement logic component
		struct Movement {
			Lua::Script script;

			Movement() : script() {}
			// Movement(std::string const& script) : script(script) {}
		};

		void registerComponents();
	}
}

#endif //!C_GAMELOGIC_HPP