/*****************************************************************//**
 * \file   sysAudio.cpp
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/sysAudio.h"
#include "Components/cAudio.h"

namespace NIKE {
	void Audio::Manager::init() {

	}

	void Audio::Manager::update() {
		for (auto& layer : NIKE_SCENES_SERVICE->getCurrScene()->getLayers()) {

			//SKip inactive layer
			if (!layer->getLayerState())
				continue;

			for (auto& entity : entities) {

				//Skip entities that are not present within layer
				if (!layer->checkEntity(entity))
					continue;

				//Manage entities with SFX audio
				if (NIKE_ECS_MANAGER->checkEntityComponent<Audio::SFX>(entity)) {
					auto& e_sfx = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);

					if (e_sfx.b_play_sfx) {
						NIKE_AUDIO_SERVICE->playAudio(e_sfx.audio_id, "", e_sfx.channel_group_id, e_sfx.volume, e_sfx.pitch, false);
						e_sfx.b_play_sfx = false;
					}
				}
			}
		}
	}
}
