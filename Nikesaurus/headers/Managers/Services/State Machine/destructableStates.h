/*****************************************************************//**
 * \file   destructableStates.h
 * \brief  Destructable States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once
#ifndef DESTRUCTABLE_STATES
#define DESTRUCTABLE_STATES

#include "Managers/Services/State Machine/sStateMachine.h"
#include "Managers/ECS/mEntity.h"
#include "Managers/ECS/mSystem.h"
#include "Components/cPhysics.h"

namespace NIKE {
	namespace State {
		class DestructableDeathState : public StateMachine::Istate
		{
		public:
			DestructableDeathState();

			void onEnter(Entity::Type& entity) override;
			void onUpdate(Entity::Type& entity) override;
			void onExit(Entity::Type& entity) override;

			/***********************
			* SFX handling
			************************/
			void playSFX(Entity::Type& entity, bool play_or_no) override;

		private:

		};
	}
}


#endif