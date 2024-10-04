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


#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysAudio.h"

/************************************
* SOME NOTES TO TAKE NOTE OF: 
* - AUDIO WILL BE PLAYED LIKE AE
* - THERE WILL BE AUIDIO GROUP AND AUDIO
/****************************************/


namespace Audio {

	struct cAudio
	{

		// Audio and audio group
		NE_AUDIO audio;

		NE_AUDIO_GROUP audio_group;

		// Bool to check if audio is played
		bool is_played = false;

		// Bool to indicate loop
		bool is_loop;

		// Volume 
		float volume;

		// Pitch
		float pitch;

		// Ctor
		cAudio(NE_AUDIO param_audio, NE_AUDIO_GROUP param_audio_group, float param_volume, float param_pitch, bool param_loop)
			: audio{ param_audio }, audio_group{ param_audio_group }, is_played{ false }, is_loop{ param_loop }, volume{ param_volume }, pitch{ param_pitch } {}
	};

}

#endif