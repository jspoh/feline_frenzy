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

		//Player logic component
		struct Player {
			std::string script;
			std::string script_id;
			bool b_loaded;

			Player() : script{ "" }, script_id { "" }, b_loaded{ false } {}
			Player(std::string const& script) : script{ script }, script_id{ "" }, b_loaded{false} {}
		};

		void registerComponents();
	}
}

#endif //!C_GAMELOGIC_HPP