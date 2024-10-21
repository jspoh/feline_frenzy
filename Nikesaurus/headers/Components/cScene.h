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

#include "Core/stdafx.h"

namespace NIKESAURUS {
	namespace Scenes {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		//Scene manager actions
		enum class NIKESAURUS_API Actions {
			CHANGE = 0,
			RESTART,
			PREVIOUS,
		};

		//Change Scene Event
		struct NIKESAURUS_API SceneEvent {
			Actions scene_action;
			std::string next_scene_id;
		};


		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif // !C_SCENES_HPP