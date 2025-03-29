/*****************************************************************//**
 * \file   cGameLogic.h
 * \brief  Game Logic components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef C_GAMELOGIC_HPP
#define C_GAMELOGIC_HPP

namespace NIKE {
	namespace GameLogic {

		//Define a Lua-compatible value type
		using LuaValue = std::variant<int, float, std::string, bool>;

		struct Script {

			//Lua values
			std::string script_id;
			std::string update_func;

			//Script configs
			std::unordered_map<std::string, LuaValue> configs;

			//Script instance
			sol::table script_instance;

			//Init flag
			bool b_init = false;

			//Constructors
			Script() = default;
			Script(std::string const& script_id, std::string const& update_func) : script_id{ script_id }, update_func{ update_func } {}
		};

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

		void registerComponents();

		void registerEditorComponents();
	}
}

#endif //!C_GAMELOGIC_HPP