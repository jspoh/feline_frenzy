/*****************************************************************//**
 * \file   sysAnimation.h
 * \brief	Animation manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Systems/Animation/sysAnimation.h"
#include "Components/cAnimation.h"
#include "Core/Engine.h"

namespace NIKE {
	void Animation::Manager::init() {
		sprite_animator = std::make_unique<Animation::SpriteSheet>();

		std::shared_ptr<Animation::Manager> animation_sys_wrapped(this, [](Animation::Manager*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Animation::AnimationEvent>(animation_sys_wrapped);
	}

	void Animation::Manager::update() {
		for (auto& entity : entities) {
			//Update camera position
			if (NIKE_ECS_MANAGER->checkEntityComponent<Animation::cSprite>(entity)) {
				sprite_animator->animateSprite(NIKE_ECS_MANAGER->getEntityComponent<Animation::cBase>(entity), NIKE_ECS_MANAGER->getEntityComponent<Animation::cSprite>(entity), NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity));
			}
		}
	}

	//Execute event
	void Animation::Manager::onEvent(std::shared_ptr<Animation::AnimationEvent> new_event) {

		//Execute event on animator
		for (auto& entity : entities) {
			if (NIKE_ECS_MANAGER->getEntityComponent<Animation::cBase>(entity).animator_id == new_event->animator_id) {
				if (new_event->animation_action == Animation::Mode::RESTART) {
					NIKE_ECS_MANAGER->getEntityComponent<Animation::cSprite>(entity).curr_index = NIKE_ECS_MANAGER->getEntityComponent<Animation::cSprite>(entity).start_index;
				}
				sprite_animator->executeEvent(new_event->animation_action, NIKE_ECS_MANAGER->getEntityComponent<Animation::cBase>(entity));
			}
		}
	}
}
