/*****************************************************************//**
 * \file   sysImgui.cpp
 * \brief  Imgui system
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/sImgui.h"
#include "Core/Engine.h"

namespace NIKESAURUS {

	//IMGUI::Service::Service()
	//{

	//}

	//Imgui::Manager::~Manager()
	//{
	//	//ImGui_ImplOpenGL3_Shutdown();
	//	//ImGui_ImplGlfw_Shutdown();
	//	//ImGui::DestroyContext();
	//}

	void IMGUI::Service::init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(std::static_pointer_cast<Windows::NIKEWindow>(NIKEEngine.getService<Windows::Service>()->getWindow())->getWindowPtr(), true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	void IMGUI::Service::cleanUp()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void IMGUI::Service::update()
	{
		float slider_value = 0.0f;
		// Middle is main game loop, watever shit we want to render
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();
		// Seting imgui IO stuff
		io.DeltaTime = NIKEEngine.getService<Windows::Service>()->getDeltaTime();
		io.DisplaySize = ImVec2(static_cast<float>(NIKEEngine.getService<Windows::Service>()->getWindow()->getWindowPos().x),
			static_cast<float>(NIKEEngine.getService<Windows::Service>()->getWindow()->getWindowPos().y));

		// Create a new ImGui window
		static bool show = 1;
		if (show)
		{
			ImGui::ShowDemoWindow(&show);
		}


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

		// THIS 2 CALL THE OPENGL DRAWING
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfwSwapBuffers(NIKEEngine.accessWindow()->getWindow());
	}

}



