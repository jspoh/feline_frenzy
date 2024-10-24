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

namespace NIKESAURUS {
	namespace Audio {

		//SFX Struct
		struct NIKESAURUS_API SFX {
			bool play_sfx;
			std::string const& audio_id;
			std::string const& channel_group_id;
			float volume;
			float pitch;

			SFX(bool play_sfx, std::string const& audio_id, std::string const& channel_group_id, float volume, float pitch)
				:play_sfx{ play_sfx }, audio_id{ audio_id }, channel_group_id{ channel_group_id }, volume{ volume }, pitch{ pitch } {}
		};
	}
}

#endif