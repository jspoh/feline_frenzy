/*****************************************************************//**
 * \file   sysAnimation.cpp
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

		//Iteration every fixed step for fixed delta time
		for (int step = 0; step < NIKE_WINDOWS_SERVICE->getCurrentNumOfSteps(); ++step) {

			//Iterate through layers
			for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {

				//SKip inactive layer
				if (!layer->getLayerState())
					continue;

				//Iterate through all entities
				for (auto& entity : layer->getEntitites()) {

					//Skip entity not registered to this system
					if (entities.find(entity) == entities.end()) continue;

					//Get base animator comp
					auto e_baseanimator_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
					if (!e_baseanimator_comp.has_value()) continue;
					auto& e_baseanimator = e_baseanimator_comp.value().get();

					//Update entities with spritesheet
					auto e_spriteanimator_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);
					auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
					if (e_spriteanimator_comp.has_value() && e_texture_comp.has_value()) {
						auto& e_spriteanimator = e_spriteanimator_comp.value().get();
						auto& e_texture = e_texture_comp.value().get();

						//Animate spritesheet
						sprite_animator->animateSprite(e_baseanimator, e_spriteanimator, e_texture);
					}
				}
			}
		}
	}
}
