/*****************************************************************//**
 * \file   sysAnimation.h
 * \brief	Animation manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/Animation/sysAnimation.h"
#include "../headers/Components/cAnimation.h"
#include "../headers/Core/Engine.h"

void Animation::Manager::init() {
	sprite_animator = std::make_unique<Animation::SpriteSheet>();
}

void Animation::Manager::update() {
	for (auto& entity : entities) {
		//Update camera position
		if (NIKEEngine.checkEntityComponent<Animation::cSprite>(entity)) {
			sprite_animator->animateSprite(NIKEEngine.getEntityComponent<Animation::cBase>(entity), NIKEEngine.getEntityComponent<Animation::cSprite>(entity), NIKEEngine.getEntityComponent<Render::Texture>(entity));
		}
	}
}

//Execute event
void Animation::Manager::executeEvent(std::shared_ptr<Animation::AnimationEvent> new_event) {

	//Execute event on animator
	for (auto& entity : entities) {
		if (NIKEEngine.getEntityComponent<Animation::cBase>(entity).animator_id == new_event->animator_id) {
			if (new_event->animation_action == Animation::Mode::RESTART) {
				NIKEEngine.getEntityComponent<Animation::cSprite>(entity).curr_index = NIKEEngine.getEntityComponent<Animation::cSprite>(entity).start_index;
			}
			sprite_animator->executeEvent(new_event->animation_action, NIKEEngine.getEntityComponent<Animation::cBase>(entity));
		}
	}
}