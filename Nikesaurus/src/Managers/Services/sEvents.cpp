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
		auto world_size = NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize();
		auto viewport_size = NIKE_WINDOWS_SERVICE->getWindow()->getViewportSize();

		//Account for gaps between game viewport and game window
		auto gaps = NIKE_WINDOWS_SERVICE->getWindow()->getViewportWindowGap();
		gaps /= 2.0f;

		//World Scale Factor
		Vector2f scale{ (float)world_size.x / ((float)viewport_size.x / NIKE_CAMERA_SERVICE->getActiveCamera().zoom), (float)world_size.y / ((float)viewport_size.y / NIKE_CAMERA_SERVICE->getActiveCamera().zoom) };

		//Calculate world mouse position
		Vector2f world_mouse_pos = { ((float)xpos - gaps.x) * scale.x , ((float)ypos - gaps.y) * scale.y };
		world_mouse_pos.x = world_mouse_pos.x - ((viewport_size.x * scale.x) / 2.0f) + NIKE_CAMERA_SERVICE->getActiveCamera().position.x;
		world_mouse_pos.y = -(world_mouse_pos.y - ((viewport_size.y * scale.y) / 2.0f) - NIKE_CAMERA_SERVICE->getActiveCamera().position.y);

		//Get mouse position
		auto mouse_pos = Vector2f(static_cast<float>(xpos), static_cast<float>(ypos));
 
		//Dispatch mouse event
		NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Input::MouseMovedEvent>(mouse_pos, world_mouse_pos));
	}

	void Events::Service::mousescroll_cb([[maybe_unused]] GLFWwindow* window, double xoffset, double yoffset) {
		NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Input::MouseScrollEvent>(Vector2f(static_cast<float>(xoffset), static_cast<float>(yoffset))));
	}

	void Events::Service::windowfocus_cb([[maybe_unused]] GLFWwindow* window, int focused) {
		NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Windows::WindowFocusEvent>(focused));
	}

	void Events::Service::dropfile_cb([[maybe_unused]] GLFWwindow* window, int count, const char** paths) {
		NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Assets::FileDropEvent>(count, paths));
	}

	void Events::Service::cursorenter_cb([[maybe_unused]] GLFWwindow* window, int entered) {
		NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Input::CursorEnterEvent>(entered));
	}

}
