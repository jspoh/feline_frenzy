#pragma once

#ifndef C_SCENES_HPP
#define C_SCENES_HPP

#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mEvents.h"

namespace Scenes {

	//Scene manager actions
	enum class Actions {
		IDLE = 0,
		CHANGE,
		RESTART,
		PREVIOUS
	};

	//Change Scene Event
	struct ChangeSceneEvent : public Events::IEvent {
	public:
		Actions scene_action;
		std::string next_scene_id;

		//Default constructor
		ChangeSceneEvent() : scene_action{ Actions::IDLE }, next_scene_id{ "" } {}

		//Constructor
		ChangeSceneEvent(Actions action, std::string next_scene)
			: scene_action{ action }, next_scene_id{ next_scene }
		{}
	};
}

#endif // !C_SCENES_HPP