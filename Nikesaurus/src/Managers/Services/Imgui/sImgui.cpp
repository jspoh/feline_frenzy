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

 //Registered Systems
#include "Systems/sysAudio.h"
#include "../headers/Systems/Physics/sysPhysics.h"
#include "../headers/Systems/Animation/sysAnimation.h"
#include "../headers/Systems/Render/sysRender.h"

namespace NIKE {

	void IMGUI::Service::init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(std::static_pointer_cast<Windows::NIKEWindow>(NIKE_WINDOWS_SERVICE->getWindow())->getWindowPtr(), true);
		ImGui_ImplOpenGL3_Init("#version 450");

		populateLists = false;
		// For testing
		// NIKE_ASSETS_SERVICE->loadTexture("test3", "assets/Textures/Tjunction.png");
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

	bool IMGUI::Service::checkEntityExist(std::string const& entity_ref)
	{
		if (entities_ref.find(entity_ref) != entities_ref.end())
		{
			return true;
		}
		return false;
	}

	void IMGUI::Service::resetVariables()
	{
		entities_ref.clear();
	}

	std::unordered_map<std::string, Entity::Type>& IMGUI::Service::getEntityRef()
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

	bool IMGUI::Service::getImguiActive() const {
		return b_show_imgui;
	}
	
	void IMGUI::Service::setGamePaused(bool pause) {
		b_pause_game = pause;

		if (b_pause_game) {
			//Set audio system pause
			NIKE_ECS_MANAGER->setSystemState<Audio::Manager>(false);

			//Set physics system pause
			NIKE_ECS_MANAGER->setSystemState<Physics::Manager>(false);

			//Set animation system pause
			NIKE_ECS_MANAGER->setSystemState<Animation::Manager>(false);

			//Set animation system pause
			//NIKE_ECS_MANAGER->setSystemState<Render::Manager>(false);
		}
		else {
			//Set audio system play
			NIKE_ECS_MANAGER->setSystemState<Audio::Manager>(true);

			//Set physics system play
			NIKE_ECS_MANAGER->setSystemState<Physics::Manager>(true);

			//Set animation system play
			NIKE_ECS_MANAGER->setSystemState<Animation::Manager>(true);

			//Set render system play
			NIKE_ECS_MANAGER->setSystemState<Render::Manager>(true);
		}
	}

	bool IMGUI::Service::getGamePaused() const {
		return b_pause_game;
	}

	void IMGUI::Service::update()
	{
		//Toggle imgui on off
		if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_TAB)) {
			b_show_imgui = !b_show_imgui;
			b_dispatch_viewport = true;
		}

		if (b_show_imgui) {
			// Main IMGUI loop
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			imguiInputUpdate();

			imguiDockingSpace();

			// Window UI functions goes here
			imguiEntityWindow();
			imguiDebuggingWindow();
			// imguiFileSystemWindow();
			imguiShowLoadedAssetsWindow();
			imguiEntityComponentManagementWindow();
			imguiShowGameViewport(b_dispatch_viewport);
			imguiCameraControl();
			imguiLayerManagementWindow();
			imguiShowLoadedLevelsWindow();

			// THIS 2 CALL THE OPENGL DRAWING
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		//Dispatch normal viewport
		if (b_dispatch_viewport) {
			//Dispatch viewport changes
			Vector2f win_pos = { 0.0f, 0.0f };
			Vector2f win_size = { (float)NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x, (float)NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y };
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<IMGUI::ViewPortEvent>(win_pos, win_size));
			b_dispatch_viewport = false;
		}
	}

}



