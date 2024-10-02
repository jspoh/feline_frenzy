/*****************************************************************//**
 * \file   sysAnimation.h
 * \brief	Animation manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
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
void Animation::Manager::executeEvent(std::shared_ptr<Events::IEvent> event) {
	Animation::AnimationEvent new_event = *std::dynamic_pointer_cast<Animation::AnimationEvent>(event);

	//Execute event on animator
	for (auto& entity : entities) {
		if (NIKEEngine.getEntityComponent<Animation::cBase>(entity).animator_id == new_event.animator_id)
			sprite_animator->executeEvent(new_event.animation_action, NIKEEngine.getEntityComponent<Animation::cBase>(entity));
	}
}