#include "Core/stdafx.h"
#include "Managers/Services/Imgui/sImgui.h"
#include "Core/Engine.h"

namespace NIKE
{
	void imguiInputUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		// Seting imgui IO stuff
		io.DeltaTime = NIKE_WINDOWS_SERVICE->getDeltaTime();

		//// Pass in input to imgui
		//io.MousePos = ImVec2(NIKE_INPUT_SERVICE->getMousePos().x, NIKE_INPUT_SERVICE->getMousePos().y);
		//io.MouseDown[0] = NIKE_INPUT_SERVICE->isMousePressed(NIKE_MOUSE_BUTTON_1);
		//io.MouseDown[1] = NIKE_INPUT_SERVICE->isMousePressed(NIKE_MOUSE_BUTTON_2);

		//// Pass mouse scroll to imgui
		//io.MouseWheelH += NIKE_INPUT_SERVICE->getMouseScroll().x;
		//io.MouseWheel += NIKE_INPUT_SERVICE->getMouseScroll().y;


		io.DisplaySize = ImVec2(static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x),
			static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y));
	}

	bool hasValidTextureExtension(const std::filesystem::path& filePath) {
		std::string extension = filePath.extension().string();
		return (extension == ".png" || extension == ".jpg" || extension == ".tex");
	}

	bool hasValidAudioExtension(const std::filesystem::path& filePath)
	{
		std::string extension = filePath.extension().string();
		return (extension == ".wav");
	}

	bool hasValidScnTxtExtension(const std::filesystem::path& filePath)
	{
		std::string extension = filePath.extension().string();
		return (extension == ".scn");
	}

	bool hasValidFontExtension(const std::filesystem::path& filePath)
	{
		std::string extension = filePath.extension().string();
		return (extension == ".ttf");
	}

	bool hasValidVertExtension(const std::filesystem::path& filePath)
	{
		std::string extension = filePath.extension().string();
		return (extension == ".frag" || extension == ".vert");
	}

	bool hasValidScriptExtension(const std::filesystem::path& filePath)
	{
		std::string extension = filePath.extension().string();
		return (extension == ".lua");
	}

	bool hasValidModelExtension(const std::filesystem::path& filePath)
	{
		std::string extension = filePath.extension().string();
		return (extension == ".txt");
	}

	bool hasValidPrefabExtension(const std::filesystem::path& filePath)
	{
		std::string extension = filePath.extension().string();
		return (extension == ".prefab");
	}

	void displayAssetList(const std::string& asset_type)
	{
		// Variable to store the selected texture and file path
		static std::string selected_texture;
		static std::string selected_file_path;

		// To track if we need to show the popup
		static bool open_entity_prefab_popup = false;
		static bool delete_file_popup = false;
		static bool delete_all_files_popup = false;
		// Stores the selected prefab name
		static std::string selected_prefab;

		// Refresh button to reload assets if needed
		if (ImGui::Button(("Refresh " + asset_type).c_str()))
		{
			NIKE_ASSETS_SERVICE->reloadAssets(asset_type);
		}

		ImGui::Separator();
		ImGui::BeginChild("Asset List", ImVec2(0, 0), true);

		if (asset_type == "Textures")
		{
			for (const auto& texture : NIKE_ASSETS_SERVICE->getLoadedTextures())
			{
				ImVec2 uv0(0.0f, 1.0f);
				ImVec2 uv1(1.0f, 0.0f);

				std::string unique_id = "##" + texture.first;
				if (ImGui::ImageButton(unique_id.c_str(), (intptr_t)texture.second->gl_data, ImVec2(64, 64), uv0, uv1)) {
					selected_texture = texture.first;
				}

				// Drag-and-drop source for the texture
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					ImGui::SetDragDropPayload("Texture", texture.first.c_str(), texture.first.size() + 1);
					ImGui::Text("Dragging %s", texture.first.c_str());
					ImGui::EndDragDropSource();
				}

				ImGui::SameLine();
				ImGui::Text("%s", texture.first.c_str());
			}
			if (!selected_texture.empty()) {

				ImGui::Begin("Selected Texture", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);
				ImGui::Text("Texture: %s", selected_texture.c_str());

				auto textureData = NIKE_ASSETS_SERVICE->getLoadedTextures().find(selected_texture);
				if (textureData != NIKE_ASSETS_SERVICE->getLoadedTextures().end()) {
					ImVec2 uv0(0.0f, 1.0f);
					ImVec2 uv1(1.0f, 0.0f);
					ImGui::Image((intptr_t)textureData->second->gl_data, ImVec2(256, 256), uv0, uv1);
				}

				if (ImGui::Button("Delete")) {
					delete_file_popup = true;
					ImGui::OpenPopup("Confirm Delete");
				}

				ImGui::SameLine();

				if (ImGui::Button("Close")) {
					selected_texture.clear();
				}

				delete_file_popup = showDeleteFilePopup(selected_texture, "Textures");

				if (delete_file_popup) {
					selected_texture.clear();
				}

				ImGui::End();
			}
		}
		else if (asset_type == "Audio")
		{
			for (const auto& audio : NIKE_ASSETS_SERVICE->getLoadedAudios())
			{
				std::string buttonLabel = "X##" + audio.first;

				if (ImGui::SmallButton(buttonLabel.c_str())) {
					selected_file_path = audio.first;
					delete_file_popup = true;
					ImGui::OpenPopup("Confirm Delete");
				}

				ImGui::SameLine();
				ImGui::Text("%s", audio.first.c_str());
				ImGui::Separator();

				
			}
			delete_file_popup = showDeleteFilePopup(selected_file_path, "Audio");
		}
		else if (asset_type == "Fonts")
		{
			for (const auto& font : NIKE_ASSETS_SERVICE->getLoadedFonts())
			{
				ImGui::Text("%s", font.first.c_str());
			}
		}
		else if (asset_type == "Models")
		{
			for (const auto& model : NIKE_ASSETS_SERVICE->getLoadedModels())
			{
				ImGui::Text("%s", model.first.c_str());
			}
		}
		else if (asset_type == "Prefabs")
		{
			if (NIKE_ASSETS_SERVICE->getLoadedPrefabs().empty()) {
				NIKE_ASSETS_SERVICE->loadPrefabFiles();
			}

			for (const auto& prefab : NIKE_ASSETS_SERVICE->getLoadedPrefabs())
			{

				std::string buttonLabel = "X##" + prefab.first;

				if (ImGui::SmallButton(buttonLabel.c_str())) {
					selected_file_path = prefab.first;
					delete_file_popup = true;
					ImGui::OpenPopup("Confirm Delete");
				}

				ImGui::SameLine();

				if (ImGui::Selectable(prefab.first.c_str(), false, ImGuiSelectableFlags_AllowOverlap)) {
					selected_prefab = prefab.first;
					open_entity_prefab_popup = true;
					ImGui::OpenPopup("Create Entity with Prefab");
				}

				ImGui::Separator();
			}

			if (ImGui::Button("Clear all Prefabs files", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
			{
				delete_all_files_popup = true;
				ImGui::OpenPopup("Confirm Deleting All Files");

			}

			open_entity_prefab_popup = showCreateEntityPrefabPopUp(selected_prefab);
			delete_file_popup = showDeleteFilePopup(selected_file_path, "Prefabs");
			delete_all_files_popup = showDeleteAllFilesPopup("Prefabs");
		}
		else if (asset_type == "Levels") {
			if (NIKE_ASSETS_SERVICE->getLevelsList().empty()) {
				NIKE_ASSETS_SERVICE->loadScnFiles();
			}

			for (const auto& level : NIKE_ASSETS_SERVICE->getLevelsList()) {


				std::string buttonLabel = "X##" + level.first; // Small button has to have unqiue id in order to delete
				if (ImGui::SmallButton(buttonLabel.c_str())) {
					selected_file_path = level.first;
					delete_file_popup = true;
					ImGui::OpenPopup("Confirm Delete");
				}

				ImGui::SameLine();

				if (ImGui::Selectable(level.first.c_str(), false, ImGuiSelectableFlags_AllowOverlap)) {
					selected_file_path = level.first;
					NIKE_IMGUI_SERVICE->getSelectedEntityName() = "";

					std::string scene_file_path = level.second.string();
					if (std::filesystem::exists(scene_file_path)) {
						NIKE_ECS_MANAGER->destroyAllEntities();
						NIKE_IMGUI_SERVICE->getEntityRef().clear();
						NIKE_SERIALIZE_SERVICE->loadSceneFromFile(scene_file_path);
						NIKE_IMGUI_SERVICE->populateLists = false;
					}
				}

				ImGui::Separator();

			}
			ImGui::Spacing();
			if (ImGui::Button("Clear all Level files", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
			{
				delete_all_files_popup = true;
				ImGui::OpenPopup("Confirm Deleting All Files");

			}
			delete_file_popup = showDeleteFilePopup(selected_file_path, "Levels");
			delete_all_files_popup = showDeleteAllFilesPopup("Levels");
		}
		else if (asset_type == "Scripts")
		{
			for (const auto& shader : NIKE_ASSETS_SERVICE->getLoadedScripts())
			{
				ImGui::Text("%s", shader.first.c_str());
			}
		}

		ImGui::EndChild();
	}

}

