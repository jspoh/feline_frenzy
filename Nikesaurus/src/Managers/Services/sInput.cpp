/*****************************************************************//**
 * \file   mInput.cpp
 * \brief  input manager for engine
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/sInput.h"

namespace NIKESAURUS {

	void Input::Service::onEvent(std::shared_ptr<KeyEvent> event) {

		//Reset all release events upon new event
		for (auto& input : input_events) {
			input.second.released = false;
		}

		//Handle event states
		switch (event->state) {
		case States::PRESS:
			input_events[event->code].pressed = true;
			input_events[event->code].triggered = true;
			input_events[event->code].released = false;
			break;
		case States::REPEAT:
			input_events[event->code].pressed = true;
			input_events[event->code].triggered = false;
			input_events[event->code].released = false;
			break;
		case States::RELEASE:
			input_events[event->code].pressed = false;
			input_events[event->code].triggered = false;
			input_events[event->code].released = true;
			break;
		default:
			break;
		}
	}

	void Input::Service::onEvent(std::shared_ptr<MouseBtnEvent> event) {
		//Reset all release events upon new event
		for (auto& input : input_events) {
			input.second.released = false;
		}

		//Handle event states
		switch (event->state) {
		case States::PRESS:
			input_events[event->code].pressed = true;
			input_events[event->code].triggered = true;
			input_events[event->code].released = false;
			break;
		case States::REPEAT:
			input_events[event->code].pressed = true;
			input_events[event->code].triggered = false;
			input_events[event->code].released = false;
			break;
		case States::RELEASE:
			input_events[event->code].pressed = false;
			input_events[event->code].triggered = false;
			input_events[event->code].released = true;
			break;
		default:
			break;
		}
	}

	void Input::Service::onEvent(std::shared_ptr<MouseMovedEvent> event) {
		mouse.pos = event->pos;
	}

	void Input::Service::onEvent(std::shared_ptr<MouseScrollEvent> event) {
		mouse.offset = event->offset;
	}

	bool Input::Service::isKeyPressed(int key) {
		return input_events[key].pressed;
	}

	bool Input::Service::isKeyTriggered(int key) {
		bool return_state = input_events[key].triggered;
		input_events[key].triggered = false;
		return return_state;
	}

	bool Input::Service::isKeyReleased(int key) {
		bool return_state = input_events[key].released;
		input_events[key].released = false;
		return return_state;
	}

	bool Input::Service::isMousePressed(int btn) {
		return input_events[btn].pressed;
	}

	bool Input::Service::isMouseTriggered(int btn) {
		bool return_state = input_events[btn].triggered;
		input_events[btn].triggered = false;
		return return_state;
	}

	bool Input::Service::isMouseReleased(int btn) {
		bool return_state = input_events[btn].released;
		input_events[btn].released = false;
		return return_state;
	}

	Vector2f Input::Service::getMousePos() const {
		return mouse.pos;
	}

	Vector2f Input::Service::getMouseScroll() const {
		return mouse.offset;
	}
}
