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

#include "Core/Engine.h"
#include "Managers/Services/State Machine/sStateMachine.h"

namespace NIKE{
	namespace State {

		struct State
		{
			std::weak_ptr<NIKE::StateMachine::Istate> current_state;
			std::string state_id;
			Entity::Type* entity_ref = nullptr;
		};

		void registerComponents();
	}
}

#endif