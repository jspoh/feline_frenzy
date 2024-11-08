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

		//Game Logic States
		enum class InnerStates {
			ENTER = 0,
			UPDATE,
			EXIT
		};

		//Script data
		struct Script {
			std::string script_id;
			std::string script_path;
			std::string function;
			bool b_loaded;

			Script() : script_id{ "" }, script_path{ "" }, function{ "" }, b_loaded{ false } {}
			Script(std::string const& script) : script_id{ "" }, script_path{ script }, function{ "" }, b_loaded{ false } {}
		};

		//State machine component
		struct StateMachine {
			InnerStates curr_state;

			//State scripts ( first one is script id, second is script path )
			std::unordered_map<InnerStates, Script> state_scripts;

			StateMachine() : curr_state{ InnerStates::ENTER } {}
			//StateMachine(InnerStates start_state) : curr_state { start_state } {}
		};

		//Player logic component
		struct Player {
			Script script;

			Player() : script() {}
			Player(std::string const& script) : script(script) {}
		};

		void registerComponents();
	}
}

#endif //!C_GAMELOGIC_HPP