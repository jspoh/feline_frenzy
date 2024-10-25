#include "Core/stdafx.h"
#include "Managers/Services/Imgui/sImgui.h"
#include "Managers/Services/Imgui/ImguiWindows.h"
#include "Core/Engine.h"

#define GET_ASSETS_PATH() NIKEEngine.getService<IMGUI::Service>()->assets_path

namespace NIKESAURUS {
	void imguiFileSystemWindow() {
		if (!std::filesystem::exists(GET_ASSETS_PATH())) {
			ImGui::Text("Assets folder does not exist.");
			return;
		}

		// Begin the window with a title
		ImGui::Begin("Assets Browser");

		// Display current path
		ImGui::Text("Current Path: %s", GET_ASSETS_PATH().string().c_str());

		// Navigate back to the parent directory if not at the root
		if (GET_ASSETS_PATH() != "C:\\Users\\User\\feline_frenzy\\Feline Frenzy\\assets") {
			if (ImGui::Button("..")) {
				GET_ASSETS_PATH() = GET_ASSETS_PATH().parent_path();
			}
		}

		// List directories and files
		if (std::filesystem::exists(GET_ASSETS_PATH()) && std::filesystem::is_directory(GET_ASSETS_PATH())) {
			for (const auto& entry : std::filesystem::directory_iterator(GET_ASSETS_PATH())) {
				const std::filesystem::path& path = entry.path();
				std::string filename = path.filename().string();

				// If it's a directory, make it clickable
				if (std::filesystem::is_directory(path)) {
					if (ImGui::Selectable((filename + "/").c_str(), false)) {
						// Update current_directory to the new path when clicking a directory
						GET_ASSETS_PATH() = path;
					}
				}
				// If it's a file with a valid extension, display it
				else if (hasValidExtension(path)) {
					ImGui::Text("%s", filename.c_str());
				}
			}
		}
		else {
			ImGui::Text("No valid directory or path.");
		}

		ImGui::End();
	}

	void imguiEntityWindow()
	{	
		// Every window starts with Imgui::begin and ends with Imgui::end
		ImGui::Begin("Components Window");
		NIKEEngine.getService<Coordinator::Service>();
		if (ImGui::Button("Create Entity")) {
			NIKEEngine.getService<Coordinator::Service>()->createEntity();
		}
		ImGui::End();
	}
}

