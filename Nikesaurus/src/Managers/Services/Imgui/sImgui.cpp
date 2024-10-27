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

	std::filesystem::path IMGUI::Service::getAssetsPath()
	{
		return assets_path;
	}

	void IMGUI::Service::setFilePath(const std::filesystem::path& path)
	{
		assets_path = path;
	}

	void IMGUI::Service::addEntityRef(const std::string& name, Entity::Type entity_type)
	{
		entities_ref[name] = entity_type;
	}

	const std::unordered_map<std::string, Entity::Type>& IMGUI::Service::getEntityRef() const
	{
		return entities_ref;
	}

	std::string& IMGUI::Service::getSelectedEntityName()
	{
		return selected_entity_name;
	}

	Entity::Type IMGUI::Service::getEntityByName(std::string const& input)
	{
		return entities_ref[input];
	}

	void IMGUI::Service::update()
	{
		// Main IMGUI loop
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		imguiInputUpdate();

		// Window UI functions goes here
		imguiEntityWindow();
		imguiFileSystemWindow();
		imguiEntityComponentManagementWindow();
		imguiRenderEntityWindow();


		// THIS 2 CALL THE OPENGL DRAWING
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

}



