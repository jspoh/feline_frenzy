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
#include "Managers/Services/mEvents.h"
#include "Managers/Services/mWindows.h"
#include "Managers/Services/mInput.h"

namespace NIKESAURUS {
	void Events::Manager::fbsize_cb([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int width, [[maybe_unused]] int height) {
		NIKEEngine.getService<Events::Manager>()->dispatchEvent(std::make_shared<Windows::WindowResized>(width, height));
	}

	void Events::Manager::key_cb([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
		NIKEEngine.getService<Events::Manager>()->dispatchEvent(std::make_shared<Input::KeyEvent>(key, action));
	}

	void Events::Manager::mousebutton_cb([[maybe_unused]] GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {
		NIKEEngine.getService<Events::Manager>()->dispatchEvent(std::make_shared<Input::MouseBtnEvent>(button, action));
	}

	void Events::Manager::mousepos_cb([[maybe_unused]] GLFWwindow* window, double xpos, double ypos) {
		NIKEEngine.getService<Events::Manager>()->dispatchEvent(std::make_shared<Input::MouseMovedEvent>(Vector2f(static_cast<float>(xpos), static_cast<float>(ypos))));
	}

	void Events::Manager::mousescroll_cb([[maybe_unused]] GLFWwindow* window, double xoffset, double yoffset) {
		NIKEEngine.getService<Events::Manager>()->dispatchEvent(std::make_shared<Input::MouseScrollEvent>(Vector2f(static_cast<float>(xoffset), static_cast<float>(yoffset))));
	}
}
