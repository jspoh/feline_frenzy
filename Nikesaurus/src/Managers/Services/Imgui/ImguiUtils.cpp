#include "Core/stdafx.h"
#include "Managers/Services/Imgui/sImgui.h"
#include "Core/Engine.h"

namespace NIKESAURUS
{
	void imguiInputUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		// Seting imgui IO stuff
		io.DeltaTime = NIKEEngine.getService<Windows::Service>()->getDeltaTime();

		//// Pass in input to imgui
		//io.MousePos = ImVec2(NIKEEngine.getService<Input::Service>()->getMousePos().x, NIKEEngine.getService<Input::Service>()->getMousePos().y);
		//io.MouseDown[0] = NIKEEngine.getService<Input::Service>()->isMousePressed(NIKE_MOUSE_BUTTON_1);
		//io.MouseDown[1] = NIKEEngine.getService<Input::Service>()->isMousePressed(NIKE_MOUSE_BUTTON_2);

		//// Pass mouse scroll to imgui
		//io.MouseWheelH += NIKEEngine.getService<Input::Service>()->getMouseScroll().x;
		//io.MouseWheel += NIKEEngine.getService<Input::Service>()->getMouseScroll().y;


		io.DisplaySize = ImVec2(static_cast<float>(NIKEEngine.getService<Windows::Service>()->getWindow()->getWindowSize().x),
			static_cast<float>(NIKEEngine.getService<Windows::Service>()->getWindow()->getWindowSize().y));
	}

	void imguiFileUI()
	{
		float slider_value{};

		// Set window pos
		ImGui::SetWindowPos(ImVec2(200, 200));

		//// Begin the window with a title
		ImGui::Begin("Simple ImGui Window");
		//// Add some text to the window
		ImGui::Text("Hello from ImGui!");
		//// Add a slider
		ImGui::SliderFloat("float", &slider_value, 0.0f, 1.0f);

		// Add a button
		if (ImGui::Button("Click me")) {
			// Handle button click event
		}

		ImGui::End(); // End the window
	}
}

