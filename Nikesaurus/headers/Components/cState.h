/*****************************************************************//**
 * \file   cState.h
 * \brief  State Machine components
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef C_FSM_H
#define C_FSM_H

#include "Managers/Services/sStateMachine.h"

namespace NIKE{
	namespace State {

		struct State
		{
			NIKE::StateMachine::Istate* curent_state = nullptr;
			std::string state_id;
		};

		void registerComponents();
	}
}

#endif