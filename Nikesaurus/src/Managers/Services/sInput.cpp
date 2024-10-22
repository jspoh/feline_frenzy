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

	//bool Input::Service::mouseTriggerCheck() {
	//	if (mouse.b_output && !b_mouse_triggered) {
	//		b_mouse_triggered = true;
	//		return true;
	//	}

	//	return false;
	//}

	//bool Input::Service::mouseReleaseCheck() {
	//	if (!mouse.b_output && !b_mouse_released) {
	//		b_mouse_released = true;
	//		return true;
	//	}

	//	return false;
	//}

	//bool Input::Service::keyTriggerCheck() {
	//	if (key.b_output && !b_key_triggered) {
	//		b_key_triggered = true;
	//		return true;
	//	}

	//	return false;
	//}

	//bool Input::Service::keyReleaseCheck() {
	//	if (!key.b_output && !b_key_released) {
	//		b_key_released = true;
	//		return true;
	//	}

	//	return false;
	//}

	//void Input::Service::update() {

	//	//Key trigger mode checking
	//	bool key_triggered = keyTriggerCheck();

	//	//Key release mode checking
	//	bool key_released = keyReleaseCheck();

	//	//Mouse trigger mode checking
	//	bool button_triggered = mouseTriggerCheck();

	//	//Mouse release mode checking
	//	bool button_released = mouseReleaseCheck();

	//	//Update all entities
	//	for (auto& entity : entities) {
	//		//Update keys
	//		if (NIKEEngine.checkEntityComponent<Input::Key>(entity)) {
	//			auto& e_key = NIKEEngine.getEntityComponent<Input::Key>(entity);
	//			e_key.key_type = key.key_type;

	//			//Update mouse based on input mode
	//			switch (e_key.mode) {
	//			case TriggerMode::CHECKED:
	//				e_key.b_output = key.b_output;
	//				break;
	//			case TriggerMode::TRIGGERED:
	//				e_key.b_output = key_triggered;
	//				break;
	//			case TriggerMode::RELEASED:
	//				e_key.b_output = key_released;
	//				break;
	//			default:
	//				break;
	//			}
	//		}

	//		//Update mouse
	//		if (NIKEEngine.checkEntityComponent<Input::Mouse>(entity)) {
	//			auto& e_mouse = NIKEEngine.getEntityComponent<Input::Mouse>(entity);
	//			e_mouse.button_pos = mouse.button_pos;
	//			e_mouse.scroll = mouse.scroll;
	//			e_mouse.button_type = mouse.button_type;

	//			//Update mouse based on input mode
	//			switch (e_mouse.mode) {
	//			case TriggerMode::CHECKED:
	//				e_mouse.b_output = mouse.b_output;
	//				break;
	//			case TriggerMode::TRIGGERED:
	//				e_mouse.b_output = button_triggered;
	//				break;
	//			case TriggerMode::RELEASED:
	//				e_mouse.b_output = button_released;
	//				break;
	//			default:
	//				break;
	//			}
	//		}

	//		if (NIKEEngine.checkEntityComponent<Transform::Runtime_Transform>(entity))
	//		{
	//			Transform::Runtime_Transform& e_runtime = NIKEEngine.getEntityComponent<Transform::Runtime_Transform>(entity);
	//			// Check if Z key is pressed for runtime scale up
	//			// X key for runtime scale down
	//			// R key is pressed for runtime rotate
	//			e_runtime.runtime_scale_up = key.b_output && (key.key_type == GLFW_KEY_Z);
	//			e_runtime.runtime_scale_down = key.b_output && (key.key_type == GLFW_KEY_X);
	//			e_runtime.runtime_rotate = key.b_output && (key.key_type == GLFW_KEY_R);
	//		}

	//		if (key.b_output && (key.key_type == GLFW_KEY_C))
	//		{
	//			try
	//			{
	//				throw std::runtime_error("crash");
	//			}
	//			catch (const std::exception&)
	//			{
	//				NIKEEngine.accessDebug()->logCrash();
	//				NIKEEngine.accessWindow()->terminate();
	//			}
	//		}

	//		//Change scene to menu ( temp )
	//		if (NIKEEngine.accessScenes()->getCurrSceneID() == "SPLASH") {
	//			if (key_triggered && (key.key_type == GLFW_KEY_ENTER))
	//			{
	//				NIKEEngine.accessScenes()->queueSceneEvent({ Scenes::Actions::CHANGE, "MENU" });
	//			}
	//		}

	//		//Change scene to performance ( temp )
	//		if (NIKEEngine.accessScenes()->getCurrSceneID() == "MENU") {
	//			if (key_triggered && (key.key_type == GLFW_KEY_ENTER))
	//			{
	//				NIKEEngine.accessScenes()->queueSceneEvent({ Scenes::Actions::CHANGE, "PERFORMANCE" });
	//			}

	//			//if (key_triggered && (key.key_type == GLFW_KEY_LEFT))
	//			//{
	//			//	NIKEEngine.accessScenes()->queueSceneEvent({ Scenes::Actions::PREVIOUS, "" });
	//			//}
	//		}

	//		//Escape program
	//		if (key.b_output && (key.key_type == GLFW_KEY_ESCAPE)) {
	//			NIKEEngine.accessWindow()->terminate();
	//		}

	//		if (key.b_output && (key.key_type == GLFW_KEY_P))
	//		{
	//			NIKEEngine.accessSystem<Audio::Service>()->NEAudioStopGroup(NIKEEngine.accessAssets()->getAudioGroup("test_group"));
	//		}
	//	}
	//}
}
