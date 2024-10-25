/*****************************************************************//**
 * \file   sysImgui.cpp
 * \brief  Imgui system
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/Imgui/sImgui.h"
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
		// Main IMGUI loop
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		imguiInputUpdate();

		// Window UI functions goes here
		imguiFileSystemWindow();
		imguiEntityWindow();


		// THIS 2 CALL THE OPENGL DRAWING
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

}



