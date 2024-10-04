/*****************************************************************//**
 * \file   sysAnimationController.cpp
 * \brief  animation controller system
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/GameLogic/sysAnimationController.h"
#include "../headers/Core/Engine.h"
#include "../headers/Components/cAnimation.h"
#include "../headers/Components/cInput.h"

void AnimationController::Manager::init() {

}

bool AnimationController::Manager::update() {

	//Loop through entities
	for (auto& entity : entities) {

		//Execute spritevent
		if (NIKEEngine.checkEntityComponent<Animation::SpriteEvent>(entity)) {
			auto& e_animate = NIKEEngine.getEntityComponent<Animation::SpriteEvent>(entity);
			auto& e_sprite = NIKEEngine.getEntityComponent<Animation::cSprite>(entity);
			auto& e_base = NIKEEngine.getEntityComponent<Animation::cBase>(entity);

			//Input checks
			if (e_animate.mouse_type == -1) {
				//Assert if key not present
				assert(NIKEEngine.checkEntityComponent<Input::Key>(entity) && "Key animate entity created without key object.");
				auto& e_key = NIKEEngine.getEntityComponent<Input::Key>(entity);

				if (e_key.b_output && e_key.key_type == e_animate.key_type && !e_animate.animation_ongoing) {
					e_animate.animation_ongoing = true;
					std::swap(e_sprite.start_index, e_animate.new_start_index);
					std::swap(e_sprite.end_index, e_animate.new_end_index);
					std::swap(e_base.animations_to_complete, e_animate.num_animations);

					auto animation_event = std::make_shared<Animation::AnimationEvent>(Animation::Mode::RESTART, e_animate.animator_id);
					NIKEEngine.accessEvents()->dispatchEvent(animation_event);
				}

				if (e_base.b_animation_finished && e_animate.animation_ongoing) {
					e_animate.animation_ongoing = false;
					std::swap(e_sprite.start_index, e_animate.new_start_index);
					std::swap(e_sprite.end_index, e_animate.new_end_index);
					std::swap(e_base.animations_to_complete, e_animate.num_animations);

					auto animation_event = std::make_shared<Animation::AnimationEvent>(Animation::Mode::RESTART, e_animate.animator_id);
					NIKEEngine.accessEvents()->dispatchEvent(animation_event);
				}
			}
			if (e_animate.key_type == -1) {
				//Assert if mouse not present
				assert(NIKEEngine.checkEntityComponent<Input::Mouse>(entity) && "Mouse animate entity created without key object.");
				auto& e_mouse = NIKEEngine.getEntityComponent<Input::Mouse>(entity);

				if (e_mouse.b_output && e_mouse.button_type == e_animate.mouse_type && !e_animate.animation_ongoing) {
					e_animate.animation_ongoing = true;
					std::swap(e_sprite.start_index, e_animate.new_start_index);
					std::swap(e_sprite.end_index, e_animate.new_end_index);
					std::swap(e_base.animations_to_complete, e_animate.num_animations);

					auto animation_event = std::make_shared<Animation::AnimationEvent>(Animation::Mode::RESTART, e_animate.animator_id);
					NIKEEngine.accessEvents()->dispatchEvent(animation_event);
				}

				if (e_base.b_animation_finished && e_animate.animation_ongoing) {
					e_animate.animation_ongoing = false;
					std::swap(e_sprite.start_index, e_animate.new_start_index);
					std::swap(e_sprite.end_index, e_animate.new_end_index);
					std::swap(e_base.animations_to_complete, e_animate.num_animations);

					auto animation_event = std::make_shared<Animation::AnimationEvent>(Animation::Mode::RESTART, e_animate.animator_id);
					NIKEEngine.accessEvents()->dispatchEvent(animation_event);
				}
			}
		}
	}

	return false;
}
