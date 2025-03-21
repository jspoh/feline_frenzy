/*****************************************************************//**
 * \file   mInput.cpp
 * \brief  input manager for engine
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/sInput.h"

namespace NIKE {

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
		mouse.window_pos = event->window_pos;
		mouse.world_pos = event->world_pos;
	}

	void Input::Service::onEvent(std::shared_ptr<MouseScrollEvent> event) {
		mouse.offset = event->offset;
	}

	void Input::Service::onEvent(std::shared_ptr<CursorEnterEvent> event) {
		cursor.cursor_entered = event->entered;
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

	Vector2f Input::Service::getMouseWindowPos() const {
		//Account for gaps between game viewport and game window
		auto gaps = NIKE_WINDOWS_SERVICE->getWindow()->getViewportWindowGap();
		gaps /= 2.0f;

		return Vector2f(mouse.window_pos.x - gaps.x, mouse.window_pos.y - gaps.y);
	}

	Vector2f Input::Service::getMouseWorldPos() const {
		auto world_size = NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize();
		auto viewport_size = NIKE_WINDOWS_SERVICE->getWindow()->getViewportSize();

		//Account for gaps between game viewport and game window
		auto gaps = NIKE_WINDOWS_SERVICE->getWindow()->getViewportWindowGap();
		gaps /= 2.0f;

		//World Scale Factor
		Vector2f scale{ (float)world_size.x / ((float)viewport_size.x / NIKE_CAMERA_SERVICE->getActiveCamera().zoom), (float)world_size.y / ((float)viewport_size.y / NIKE_CAMERA_SERVICE->getActiveCamera().zoom) };

		//Calculate world mouse position
		Vector2f world_mouse_pos = { ((float)mouse.window_pos.x - gaps.x) * scale.x , ((float)mouse.window_pos.y - gaps.y) * scale.y };
		world_mouse_pos.x = world_mouse_pos.x - ((viewport_size.x * scale.x) / 2.0f) + NIKE_CAMERA_SERVICE->getActiveCamera().position.x;
		world_mouse_pos.y = -(world_mouse_pos.y - ((viewport_size.y * scale.y) / 2.0f) - NIKE_CAMERA_SERVICE->getActiveCamera().position.y);

		return world_mouse_pos;
	}

	Vector2f Input::Service::getMouseScroll() const {
		return mouse.offset;
	}

	int Input::Service::getCursorEntererd() const {
		return cursor.cursor_entered;
	}

	int Input::Service::getCrosshair() const {
		return cursor.is_crosshair;
	}

	void Input::Service::setCrosshair(bool is_crosshair) {
		cursor.is_crosshair = is_crosshair;
	}

}
