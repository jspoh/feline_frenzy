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
		sprite_animator = std::make_unique<Animation::SpriteAnimator>();
	}

	void Animation::Manager::update() {
		//Iterate through layers
		for (auto& layer : NIKE_SCENES_SERVICE->getCurrScene()->getLayers()) {
			//SKip inactive layer
			if (!layer->getLayerState())
				continue;

			//Iterate through all entities
			for (auto& entity : entities) {

				//Skip entities that are not present within layer, or doesnt have the base animators
				if (layer->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity) || !NIKE_ECS_MANAGER->checkEntityComponent<Animation::Base>(entity))
					continue;

				//Update entities with spritesheet
				if (NIKE_ECS_MANAGER->checkEntityComponent<Animation::Sprite>(entity) && NIKE_ECS_MANAGER->checkEntityComponent<Render::Texture>(entity)) {
					auto& e_baseanimator = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
					auto& e_spriteanimator = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);
					auto& e_texture = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);

					//Animate spritesheet
					sprite_animator->animateSprite(e_baseanimator, e_spriteanimator, e_texture);
				}
			}
		}
	}
}
