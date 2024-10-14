/*****************************************************************//**
 * \file   cScene.h
 * \brief
 *
 * \author 
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef C_SCENES_HPP
#define C_SCENES_HPP

#include "../headers/Core/stdafx.h"

namespace Scenes {

	//Scene manager actions
	enum class Actions {
		CHANGE = 0,
		RESTART,
		PREVIOUS,
	};

	//Change Scene Event
	struct SceneEvent {
		Actions scene_action;
		std::string next_scene_id;
	};
}

#endif // !C_SCENES_HPP