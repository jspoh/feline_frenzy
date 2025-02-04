/*****************************************************************//**
 * \file   cAudio.h
 * \brief  Audio components for ecs architecture
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef C_AUDIO_HPP
#define C_AUDIO_HPP

#include "Core/stdafx.h"
#include <vector>

namespace NIKE {
    namespace Audio {

        // Game pause event
        struct PausedEvent : public Events::IEvent {
            bool b_game_state;

            PausedEvent(bool b_game_state) : b_game_state{ b_game_state } {}
        };

        // For additional SFX
        /*struct SFXEntry {
            bool b_play_sfx;
            std::string audio_id;
            std::string channel_group_id;
            float volume;
            float pitch;

            SFXEntry()
                : b_play_sfx(false), audio_id(""), channel_group_id(""), volume(1.0f), pitch(1.0f) {
            }

            SFXEntry(bool play, std::string const& id, std::string const& group, float vol, float p)
                : b_play_sfx(play), audio_id(id), channel_group_id(group), volume(vol), pitch(p) {
            }

            // Comparison operators (needed for serialization overrides)
            bool operator==(const SFXEntry& other) const {
                return b_play_sfx == other.b_play_sfx &&
                    audio_id == other.audio_id &&
                    channel_group_id == other.channel_group_id &&
                    volume == other.volume &&
                    pitch == other.pitch;
            }
            bool operator!=(const SFXEntry& other) const {
                return !(*this == other);
            }
        };*/

        // SFX Component (main SFX)
        struct SFX {
            bool b_play_sfx;
            std::string audio_id;
            std::string channel_group_id;
            float volume;
            float pitch;

            // Hold additional SFX file names.
            std::vector<std::string> sfx_list;

            // Default constructor initializes members and sets sfx_list with the default audio_id.
            SFX()
                : b_play_sfx(false),
                audio_id(""),
                channel_group_id(""),
                volume(1.0f),
                pitch(1.0f),
                sfx_list{ "" } // initialize with one element: the default audio_id (which is "")
            {}

            // Parameterized constructor initializes members and sets sfx_list with the provided audio_id.
            SFX(bool play, std::string const& audio_id, std::string const& channel_group_id, float volume, float pitch)
                : b_play_sfx(play),
                audio_id(audio_id),
                channel_group_id(channel_group_id),
                volume(volume),
                pitch(pitch),
                sfx_list{ audio_id } // initialize with one element: the passed-in audio_id
            {}
        };

        void registerComponents();
        void registerEditorComponents();
    }
}

#endif
