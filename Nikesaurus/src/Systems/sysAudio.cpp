/*****************************************************************//**
 * \file   sysAudio.cpp
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (70%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (30%)
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
		for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {

			//Skip inactive layer
			if (!layer->getLayerState())
				continue;

			for (auto& entity : layer->getEntitites()) {

				//If there is sfx value
				if (auto e_sfx_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity); e_sfx_comp.has_value()) {
					//Get sfx comp ref
					auto& e_sfx = e_sfx_comp.value().get();

					//Play SFX
					if (e_sfx.b_play_sfx && NIKE_ASSETS_SERVICE->isAssetRegistered(e_sfx.audio_id) && !e_sfx.channel_group_id.empty()) {

						//Check if channel group exists, else create one
						if (!NIKE_AUDIO_SERVICE->checkChannelGroupExist(e_sfx.channel_group_id)) {
							NIKE_AUDIO_SERVICE->createChannelGroup(e_sfx.channel_group_id);
						}

						//Play SFX
						NIKE_AUDIO_SERVICE->playAudio(e_sfx.audio_id, "", e_sfx.channel_group_id, e_sfx.volume, e_sfx.pitch, false, false);
						e_sfx.b_play_sfx = false;
					}
					else {

						//Set play sfx to false
						e_sfx.b_play_sfx = false;
					}
				}
			}
		}

		// Handle Playlists
		for (auto& channels : NIKE_AUDIO_SERVICE->getAllChannelGroups()) {

			if (!channels.second->isPlaying() && !channels.second->getPaused()) {
				const auto& playlist = NIKE_AUDIO_SERVICE->getChannelPlaylist(channels.first);
				if (!playlist.tracks.empty()) {
					const std::string& audio_id = std::move(playlist.tracks.front());
					
					NIKE_AUDIO_SERVICE->playAudio(audio_id, "", channels.first, 1.f, 1.f, false, true);

					if (NIKE_AUDIO_SERVICE->isPlaylistLooping(channels.first)) {
						NIKE_AUDIO_SERVICE->queueAudioToPlaylist(channels.first, audio_id);
					}
					NIKE_AUDIO_SERVICE->popAudioFromPlaylist(channels.first);
				}

			}
		}
	}
}
