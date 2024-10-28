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

namespace NIKESAURUS {
	void Audio::Manager::init() {

	}

	void Audio::Manager::update() {
		for (auto& layer : NIKEEngine.getService<Scenes::Service>()->getCurrScene()->getLayers()) {
			for (auto& entity : entities) {
				if (!layer->checkEntity(entity))
					continue;

				//Manage entities with SFX audio
				if (NIKEEngine.getService<Coordinator::Manager>()->checkEntityComponent<Audio::SFX>(entity)) {
					auto& e_sfx = NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Audio::SFX>(entity);

					if (e_sfx.play_sfx) {
						NIKEEngine.getService<Audio::Service>()->playAudio(e_sfx.audio_id, "", e_sfx.channel_group_id, e_sfx.volume, e_sfx.pitch, false);
						e_sfx.play_sfx = false;
					}
				}
			}
		}
	}
}
