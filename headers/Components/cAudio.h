/*****************************************************************//**
 * \file   cAudio.h
 * \brief  Audio components for ecs architecture
 *
 * \author Bryan Lim
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef C_AUDIO_HPP
#define C_AUDIO_HPP


#include "../headers/Core/stdafx.h"

/************************************
* SOME NOTES TO TAKE NOTE OF: 
* - AUDIO WILL BE PLAYED LIKE AE
* - THERE WILL BE AUIDIO GROUP AND AUDIO
/****************************************/


namespace Audio {

	struct cAudio
	{

		// Tag for audio and audio group
		std::string audio_tag;

		std::string audio_tag_group;

		// Bool to check if audio is played
		bool is_played = false;

		// Volume 
		float volume{};

		// Pitch
		float pitch{};
	};

}

#endif