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
				ImGui::Begin("Selected Texture");
				ImGui::Text("Texture: %s", selected_texture.c_str());

				auto textureData = NIKE_ASSETS_SERVICE->getLoadedTextures().find(selected_texture);
				if (textureData != NIKE_ASSETS_SERVICE->getLoadedTextures().end()) {
					ImVec2 uv0(0.0f, 1.0f);
					ImVec2 uv1(1.0f, 0.0f);
					ImGui::Image((intptr_t)textureData->second->gl_data, ImVec2(256, 256), uv0, uv1);
				}

				if (ImGui::Button("Close")) {
					selected_texture.clear();
				}
				ImGui::End();
			}
		}
		else if (asset_type == "Audio")
		{
			for (const auto& audio : NIKE_ASSETS_SERVICE->getLoadedAudios())
			{
				ImGui::Text("%s", audio.first.c_str());
			}
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
		else if (asset_type == "Shaders")
		{
			for (const auto& shader : NIKE_ASSETS_SERVICE->getLoadedShaders())
			{
				ImGui::Text("%s", shader.first.c_str());
			}
		}

		ImGui::EndChild();
	}

	void renderGrid(Vector2i grid_dimen, Vector2f viewport_size)
	{
		// Get the position and size of the viewport window
		ImVec2 start_pos = ImGui::GetWindowPos();

		// Debug: Log viewport and grid dimensions
		//cout << "Viewport Size (x, y): " << viewport_size.x << ", " << viewport_size.y
		//    << " Grid Dimensions (x, y): " << grid_dimen.x << ", " << grid_dimen.y
		//    << endl;

		// Calculate the tile size to ensure the grid fits inside the viewport
		float tile_size_x = viewport_size.x / grid_dimen.x;
		float tile_size_y = viewport_size.y / grid_dimen.y;
		float tile_size = min(tile_size_x, tile_size_y);

		cout << "Tile Size: " << tile_size << endl;

		// Loop through grid rows and columns to draw the squares
		for (int row = 0; row < grid_dimen.y; ++row) {
			for (int col = 0; col < grid_dimen.x; ++col) {
				// Calculate the top-left and bottom-right coordinates for each grid square
				ImVec2 top_left(start_pos.x + col * tile_size, start_pos.y + row * tile_size);
				ImVec2 bottom_right(start_pos.x + (col + 1) * tile_size, start_pos.y + (row + 1) * tile_size);

				// Draw each grid square 
				ImGui::GetWindowDrawList()->AddRect(top_left, bottom_right, IM_COL32(255, 255, 255, 255));
			}
		}
	}

	bool isMouseOverEntity(const Vector2f& mouse_pos, const Transform::Transform& transform) {
		// Assuming the entity has a size component (e.g., width and height) for hit detection
		float half_width = transform.scale.x / 2.0f;
		float half_height = transform.scale.y / 2.0f;

		// Check if mouse is within the entity's bounds
		return mouse_pos.x >= (transform.position.x - half_width) &&
			mouse_pos.x <= (transform.position.x + half_width) &&
			mouse_pos.y >= (transform.position.y - half_height) &&
			mouse_pos.y <= (transform.position.y + half_height);

	}

	void handleEntitySelectionAndDrag(const Vector2f& main_mouse) {
		// Select entity if mouse click on entity
		if (ImGui::IsMouseClicked(0)) {
			for (auto& entity : NIKE_IMGUI_SERVICE->getEntityRef()) {
				if (NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity.second))
				{
					auto& transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity.second).value().get();
					if (isMouseOverEntity(main_mouse, transform_comp)) {
						NIKE_IMGUI_SERVICE->setSelectedEntityName(entity.first);
						break;
					}
				}

			}
		}

		// Update entity's position when mouse is moving and the mouse is held down
		if (ImGui::IsMouseDown(0)) {
			std::string selected_entity_name = NIKE_IMGUI_SERVICE->getSelectedEntityName();
			if (!selected_entity_name.empty()) {
				Entity::Type entity = NIKE_IMGUI_SERVICE->getEntityByName(selected_entity_name);
				if (NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity)) {
					auto& transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity).value().get();
					transform_comp.position = main_mouse;
				}
			}
		}

		if (ImGui::IsMouseReleased(0)) {
			// NIKE_IMGUI_SERVICE->setSelectedEntityName("");
		}
	}








}

