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
        static bool open_create_entity_popup = false;  
        // Stores the selected prefab name
        static std::string selected_prefab;             

        // Refresh button to reload assets if needed
        if (ImGui::Button(("Refresh " + asset_type).c_str()) && asset_type != "Shaders")
        {
            NIKE_ASSETS_SERVICE->reloadAssets(asset_type);
        }

        ImGui::Separator();

        ImGui::BeginChild("Asset List", ImVec2(0, 0), true);

        // Check asset type and retrieve appropriate list of loaded assets
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

                // Initiate drag-and-drop source for the texture
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    // Set payload as the texture name to pass to the target
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

                // Retrieve the texture data to display it
                auto textureData = NIKE_ASSETS_SERVICE->getLoadedTextures().find(selected_texture);
                if (textureData != NIKE_ASSETS_SERVICE->getLoadedTextures().end()) {
                    // Adjust size as needed
                    // Bottom-left
                    ImVec2 uv0(0.0f, 1.0f);
                    // Top-right
                    ImVec2 uv1(1.0f, 0.0f);
                    ImGui::Image((intptr_t)textureData->second->gl_data, ImVec2(256, 256), uv0, uv1);
                }

                if (ImGui::Button("Close")) {
                    // Clear selection when closing
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
			for (const auto& prefab : NIKE_ASSETS_SERVICE->getLoadedPrefabs())
			{
				const std::string& prefab_name = prefab.first;

				if (ImGui::Selectable(prefab_name.c_str())) {
					selected_prefab = prefab_name;
					open_create_entity_popup = true;
					ImGui::OpenPopup("Create Entity with Prefab");
				}
			}

            // Open pop up to create entity with prefab
            open_create_entity_popup = showCreateEntityPrefabPopUp(selected_prefab);
        }
        else if (asset_type == "Shaders")
        {
            for (const auto& shader : NIKE_ASSETS_SERVICE->getLoadedShaders())
            {
                ImGui::Text("%s", shader.first.c_str());
            }
        }
        else if (asset_type == "Levels") {
            // Load levels list if empty
            if (NIKE_ASSETS_SERVICE->getLevelsList().empty()) {
                NIKE_ASSETS_SERVICE->loadScnFiles();
            }

            ImGui::SameLine();

            if (ImGui::Button("Clear all Level files"))
            {
                if (NIKE_ASSETS_SERVICE->deleteAllFiles(NIKE_ASSETS_SERVICE->getScenesPath()))
                {
                    NIKE_ASSETS_SERVICE->loadScnFiles();
                }
            }

            // Display loaded .scn files with selectable items
            for (const auto& level : NIKE_ASSETS_SERVICE->getLevelsList()) {
                if (ImGui::Selectable(level.first.c_str())) {
                    selected_file_path = level.first;
                    NIKE_IMGUI_SERVICE->getSelectedEntityName() = "";

                    // Ensure the file exists before attempting to load it
                    std::string scene_file_path = level.second.string();
                    if (std::filesystem::exists(scene_file_path)) {
                        // Clear previous scene entities before loading the new one
                        NIKE_ECS_MANAGER->destroyAllEntities();
                        NIKE_IMGUI_SERVICE->getEntityRef().clear();

                        // Load the scene from the selected file path
                        NIKE_SERIALIZE_SERVICE->loadSceneFromFile(scene_file_path);
                        NIKE_IMGUI_SERVICE->populateLists = false;
                    }
                }

                // Display "Remove File" button next to each selectable item
                if (ImGui::Button(("Remove " + level.first).c_str())) {
                    // Open a confirmation popup when "Remove File" button is clicked
                    selected_file_path = level.first;
                    ImGui::OpenPopup("Confirm Delete");
                }
            }

            // Confirmation popup for deleting a file
            if (ImGui::BeginPopupModal("Confirm Delete", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Are you sure you want to delete this file?");
                ImGui::Separator();

                if (ImGui::Button("Yes")) {
                    // Attempt to delete the selected file
                    std::string scene_file_path = NIKE_ASSETS_SERVICE->getLevelsList().at(selected_file_path).string();
                    if (NIKE_ASSETS_SERVICE->deleteFile(scene_file_path)) {
                        // Refresh levels list after deletion
                        NIKE_ASSETS_SERVICE->loadScnFiles();
                    }
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button("No")) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        ImGui::EndChild();
    }



}

