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

	//void Events::key_cb([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
	//	//Key triggered
	//	getInstance()->key.key_type = key;

	//	//Output result
	//	if (action == GLFW_PRESS) {
	//		getInstance()->key.b_output = true;
	//	}
	//	else if (action == GLFW_RELEASE) {
	//		getInstance()->key.b_output = false;
	//	}

	//	//Triggered reset mode
	//	if (action == GLFW_RELEASE)
	//		getInstance()->b_key_triggered = false;

	//	//Released reset mode
	//	if (action != GLFW_RELEASE)
	//		getInstance()->b_key_released = false;
	//}

	//void Events::mousebutton_cb([[maybe_unused]] GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {

	//	//Button triggered
	//	getInstance()->mouse.button_type = button;

	//	//Output result
	//	if (action == GLFW_PRESS) {
	//		getInstance()->mouse.b_output = true;
	//	}
	//	else if (action == GLFW_RELEASE) {
	//		getInstance()->mouse.b_output = false;
	//	}

	//	//Triggered reset mode
	//	if (action == GLFW_RELEASE)
	//		getInstance()->b_mouse_triggered = false;

	//	//Released reset mode
	//	if (action != GLFW_RELEASE)
	//		getInstance()->b_mouse_released = false;
	//}

	//void Events::mousepos_cb([[maybe_unused]] GLFWwindow* window, double xpos, double ypos) {
	//	getInstance()->mouse.button_pos.x = static_cast<float>(xpos);
	//	getInstance()->mouse.button_pos.y = static_cast<float>(ypos);

	//	NIKEEngine.getService<Events::Manager>()->dispatchEvent(std::make_shared<Windows::WindowResized>(width, height));
	//}

	//void Events::mousescroll_cb([[maybe_unused]] GLFWwindow* window, double xoffset, double yoffset) {
	//	getInstance()->mouse.scroll.x = static_cast<float>(xoffset);
	//	getInstance()->mouse.scroll.y = static_cast<float>(yoffset);

	//	NIKEEngine.getService<Events::Manager>()->dispatchEvent(std::make_shared<Windows::WindowResized>(width, height));
	//}
}
