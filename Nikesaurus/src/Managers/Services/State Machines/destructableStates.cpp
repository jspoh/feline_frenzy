/*****************************************************************//**
 * \file   destructableStates.cpp
 * \brief  Destructable States
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/State Machine/enemyUtils.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Core/Engine.h"
#include "Managers/Services/State Machine/destructableStates.h"

namespace NIKE {
	State::DestructableDeathState::DestructableDeathState()
	{
		// Add transitions here
	}

	void State::DestructableDeathState::onEnter([[maybe_unused]] Entity::Type& entity)
	{

	}

	void State::DestructableDeathState::onUpdate([[maybe_unused]] Entity::Type& entity)
	{
		//auto animation_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
		//if (animation_comp.has_value())
		//{
		//	// Use delta time to let animation play before deleting entity
		//	static float dt = 0.0f;
		//	dt += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
		//	if (dt >= 0.2f) {
		//		NIKE_METADATA_SERVICE->destroyEntity(entity);
		//		// Reset delta time
		//		dt = 0.f;
		//	}
		//}
		NIKE_METADATA_SERVICE->destroyEntity(entity);
	}

	void State::DestructableDeathState::onExit([[maybe_unused]] Entity::Type& entity)
	{

	}
	void State::DestructableDeathState::playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
	{

	}
}