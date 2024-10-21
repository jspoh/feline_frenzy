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

namespace NIKESAURUS {
	void Animation::Manager::init() {
		sprite_animator = std::make_unique<Animation::SpriteSheet>();
	}

	void Animation::Manager::update() {
		for (auto& entity : entities) {
			//Update camera position
			if (NIKEEngine.getService<Coordinator::Manager>()->checkEntityComponent<Animation::cSprite>(entity)) {
				sprite_animator->animateSprite(NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Animation::cBase>(entity), NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Animation::cSprite>(entity), NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Render::Texture>(entity));
			}
		}
	}

	//Execute event
	void Animation::Manager::executeEvent(std::shared_ptr<Animation::AnimationEvent> new_event) {

		//Execute event on animator
		for (auto& entity : entities) {
			if (NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Animation::cBase>(entity).animator_id == new_event->animator_id) {
				if (new_event->animation_action == Animation::Mode::RESTART) {
					NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Animation::cSprite>(entity).curr_index = NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Animation::cSprite>(entity).start_index;
				}
				sprite_animator->executeEvent(new_event->animation_action, NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Animation::cBase>(entity));
			}
		}
	}
}
