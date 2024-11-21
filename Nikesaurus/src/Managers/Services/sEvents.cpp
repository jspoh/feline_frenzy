/*****************************************************************//**
 * \file   mEvents.cpp
 * \brief  Events manager for ECS architecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/sEvents.h"
#include "Managers/Services/sWindows.h"
#include "Managers/Services/sInput.h"

namespace NIKE {
	void Events::IEvent::setEventProcessed(bool processed) {
		b_event_processed = processed;
	}

	bool Events::IEvent::getEventProcessed() const {
		return b_event_processed;
	}

	void Events::Service::fbsize_cb([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int width, [[maybe_unused]] int height) {
		NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Windows::WindowResized>(width, height));
	}

	void Events::Service::key_cb([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
		NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Input::KeyEvent>(key, action));
	}

	void Events::Service::mousebutton_cb([[maybe_unused]] GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {
		NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Input::MouseBtnEvent>(button, action));
	}

	void Events::Service::mousepos_cb([[maybe_unused]] GLFWwindow* window, double xpos, double ypos) {
		NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Input::MouseMovedEvent>(Vector2f(static_cast<float>(xpos), static_cast<float>(ypos))));
	}

	void Events::Service::mousescroll_cb([[maybe_unused]] GLFWwindow* window, double xoffset, double yoffset) {
		NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Input::MouseScrollEvent>(Vector2f(static_cast<float>(xoffset), static_cast<float>(yoffset))));
	}

	void Events::Service::windowfocus_cb([[maybe_unused]] GLFWwindow* window, int focused) {
		NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Windows::WindowFocusEvent>(focused));
	}
}
