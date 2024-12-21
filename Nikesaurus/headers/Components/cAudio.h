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

namespace NIKE {
	namespace Audio {

		// Game pause event
		struct PausedEvent : public Events::IEvent {
			bool b_game_state;

			PausedEvent(bool b_game_state) : b_game_state{ b_game_state } {}
		};

		//SFX Struct
		struct SFX {
			bool b_play_sfx;
			std::string audio_id;
			std::string channel_group_id;
			float volume;
			float pitch;

			SFX() :b_play_sfx{ false }, audio_id{ "" }, channel_group_id{ "" }, volume{ 1.0f }, pitch{ 1.0f } {}
			SFX(bool b_play_sfx, std::string const& audio_id, std::string const& channel_group_id, float volume, float pitch)
				:b_play_sfx{ b_play_sfx }, audio_id{ audio_id }, channel_group_id{ channel_group_id }, volume{ volume }, pitch{ pitch } {}
		};

		void registerComponents();
	}
}

#endif
