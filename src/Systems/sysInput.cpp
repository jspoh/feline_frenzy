/*****************************************************************//**
 * \file   sysInput.cpp
 * \brief  input manager for engine
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysInput.h"
#include "../headers/Components/cTransform.h"



void Input::Manager::setupEventCallbacks() {
	glfwSetFramebufferSizeCallback(NIKEEngine.getWindow(), fbsize_cb);
	glfwSetKeyCallback(NIKEEngine.getWindow(), key_cb);
	glfwSetMouseButtonCallback(NIKEEngine.getWindow(), mousebutton_cb);
	glfwSetCursorPosCallback(NIKEEngine.getWindow(), mousepos_cb);
	glfwSetScrollCallback(NIKEEngine.getWindow(), mousescroll_cb);
}

void Input::Manager::init() {
	//Setup event callbacks
	setupEventCallbacks();

	//Setup input mode
	glfwSetInputMode(NIKEEngine.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	mouse.mode = Input::TriggerMode::TRIGGERED;
}

bool Input::Manager::mouseTriggerCheck() {
	if (mouse.b_output && !b_mouse_triggered) {
		b_mouse_triggered = true;
		return true;
	}

	return false;
}

bool Input::Manager::mouseReleaseCheck() {
	if (!mouse.b_output && !b_mouse_released) {
		b_mouse_released = true;
		return true;
	}

	return false;
}

bool Input::Manager::keyTriggerCheck() {
	if (key.b_output && !b_key_triggered) {
		b_key_triggered = true;
		return true;
	}

	return false;
}

bool Input::Manager::keyReleaseCheck() {
	if (!key.b_output && !b_key_released) {
		b_key_released = true;
		return true;
	}

	return false;
}

void Input::Manager::update() {

	//Key trigger mode checking
	bool key_triggered = keyTriggerCheck();

	//Key release mode checking
	bool key_released = keyReleaseCheck();

	//Mouse trigger mode checking
	bool button_triggered = mouseTriggerCheck();

	//Mouse release mode checking
	bool button_released = mouseReleaseCheck();

	//Update all entities
	for (auto& entity : entities) {
		//Update keys
		if (NIKEEngine.checkEntityComponent<Input::Key>(entity)) {
			auto& e_key = NIKEEngine.getEntityComponent<Input::Key>(entity);
			e_key.key_type = key.key_type;

			//Update mouse based on input mode
			switch (e_key.mode) {
			case TriggerMode::CHECKED:
				e_key.b_output = key.b_output;
				break;
			case TriggerMode::TRIGGERED:
				e_key.b_output = key_triggered;
				break;
			case TriggerMode::RELEASED:
				e_key.b_output = key_released;
				break;
			default:
				break;
			}
		}

		//Update mouse
		if (NIKEEngine.checkEntityComponent<Input::Mouse>(entity)) {
			auto& e_mouse = NIKEEngine.getEntityComponent<Input::Mouse>(entity);
			e_mouse.button_pos = mouse.button_pos;
			e_mouse.scroll = mouse.scroll;
			e_mouse.button_type = mouse.button_type;

			//Update mouse based on input mode
			switch (e_mouse.mode) {
			case TriggerMode::CHECKED:
				e_mouse.b_output = mouse.b_output;
				break;
			case TriggerMode::TRIGGERED:
				e_mouse.b_output = button_triggered;
				break;
			case TriggerMode::RELEASED:
				e_mouse.b_output = button_released;
				break;
			default:
				break;
			}
		}

		else if (NIKEEngine.checkEntityComponent<Transform::Runtime_Transform>(entity))
		{
			Transform::Runtime_Transform& e_runtime = NIKEEngine.getEntityComponent<Transform::Runtime_Transform>(entity);
			// Check if S key is pressed for runtime scale, R key is pressed for runtime rotate
			e_runtime.runtime_scale_up = key.b_output && (key.key_type == GLFW_KEY_Z);
			e_runtime.runtime_scale_down = key.b_output && (key.key_type == GLFW_KEY_X);
			e_runtime.runtime_rotate = key.b_output && (key.key_type == GLFW_KEY_R);
		}


		// Checking for Move component
		else if (NIKEEngine.checkEntityComponent<Move::Movement>(entity)) {
			// Ref to Move component
			Move::Movement& move = NIKEEngine.getEntityComponent<Move::Movement>(entity);

			//Check if escape key is pressed
			move.Up = key.b_output && (key.key_type == GLFW_KEY_W);
			move.Left = key.b_output && (key.key_type == GLFW_KEY_A);
			move.Down = key.b_output && (key.key_type == GLFW_KEY_S);
			move.Right = key.b_output && (key.key_type == GLFW_KEY_D);
		}


	}

	//Temp changing scene here
	if (mouse.b_output && (mouse.button_type == GLFW_MOUSE_BUTTON_LEFT)) {
		auto menu_event = std::make_shared<Scenes::ChangeSceneEvent>(Scenes::Actions::CHANGE, "MENU");
		NIKEEngine.dispatchEvent(menu_event);
	}

	// !TODO make sure cannot use outside of MenuScene
	if (mouse.b_output == true && (mouse.button_type == GLFW_MOUSE_BUTTON_RIGHT)) {
		auto render_manager = Render::Manager::getInstance();
		auto camera_system = render_manager->getCamEntity();
		float mouseX = mouse.button_pos.x;
		float mouseY = mouse.button_pos.y;
		float ndcX = (2.0f * mouse.button_pos.x) / NIKEEngine.getWindowSize().x - 1.0f;
		float ndcY = 1.0f - (2.0f * mouseY) / NIKEEngine.getWindowSize().y;

		Matrix33::Matrix_33 ndc_to_world_xform;
		Matrix33::Matrix_33Inverse(ndc_to_world_xform, camera_system->getWorldToNDCXform());

		Vector3 ndc_coords = { ndcX, ndcY, 1.0f }; // NDC in homogeneous coordinates
		Vector3 world_coords = ndc_coords * ndc_to_world_xform;

		Entity::Type entity = NIKEEngine.createEntity();
		NIKEEngine.addEntityComponentObj<Render::Mesh>(entity, { "tex", "square-texture", Matrix33::Matrix_33::Identity(), "tree" });
		NIKEEngine.addEntityComponentObj<Transform::Transform>(entity, { {world_coords.x ,world_coords.y}, {200.f, 200.f}, 0.0f });
		NIKEEngine.addEntityComponentObj<Render::Color>(entity, { {1.0f, 1.0f, 1.0f}, 1.0f });

		mouse.b_output = false;

	}


	//Check if escape key is pressed
	if (key.b_output && (key.key_type == GLFW_KEY_ESCAPE)) {
		NIKEEngine.terminate();
	}


}

void Input::Manager::fbsize_cb([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int width, [[maybe_unused]] int height) {

}

void Input::Manager::key_cb([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
	//Key triggered
	getInstance()->key.key_type = key;

	//Output result
	if (action == GLFW_PRESS) {
		getInstance()->key.b_output = true;
	}
	else if (action == GLFW_RELEASE) {
		getInstance()->key.b_output = false;
	}
	
	//Triggered reset mode
	if (action == GLFW_RELEASE)
		getInstance()->b_key_triggered = false;

	//Released reset mode
	if (action != GLFW_RELEASE)
		getInstance()->b_key_released = false;
}

void Input::Manager::mousebutton_cb([[maybe_unused]] GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {

	//Button triggered
	getInstance()->mouse.button_type = button;

	//Output result
	if (action == GLFW_PRESS) {
		getInstance()->mouse.b_output = true;
	}
	else if (action == GLFW_RELEASE) {
		getInstance()->mouse.b_output = false;
	}

	//Triggered reset mode
	if (action == GLFW_RELEASE)
		getInstance()->b_mouse_triggered = false;

	//Released reset mode
	if (action != GLFW_RELEASE)
		getInstance()->b_mouse_released = false;
}

void Input::Manager::mousepos_cb([[maybe_unused]] GLFWwindow* window, double xpos, double ypos) {
	getInstance()->mouse.button_pos.x = static_cast<float>(xpos);
	getInstance()->mouse.button_pos.y = static_cast<float>(ypos);
}

void Input::Manager::mousescroll_cb([[maybe_unused]] GLFWwindow* window, double xoffset, double yoffset) {
	getInstance()->mouse.scroll.x = static_cast<float>(xoffset);
	getInstance()->mouse.scroll.y = static_cast<float>(yoffset);
}
