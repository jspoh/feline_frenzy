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
        return (extension == ".txt" || extension == ".scn");
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

    void displayAssetList(const std::string& asset_type)
    {

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
            // Retrieve loaded textures
            for (const auto& texture : NIKE_ASSETS_SERVICE->getLoadedTextures())
            {
                // Display the texture thumbnail
                ImGui::Image((intptr_t)texture.second->gl_data, ImVec2(64, 64));  

                // Display the texture name
                ImGui::SameLine();
                ImGui::Text("%s", texture.first.c_str());

                // Hover effect with the texture path as tooltip
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Texture path: %s", texture.first.c_str());
                }
            }
        }
        else if (asset_type == "Audio")
        {
            for (const auto& audio : NIKE_ASSETS_SERVICE->getLoadedAudios())
            {
                ImGui::Text("%s", audio.first.c_str()); 
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Audio path: %s", audio.first.c_str());
                }
            }
        }
        else if (asset_type == "Fonts")
        {
            for (const auto& font : NIKE_ASSETS_SERVICE->getLoadedFonts())
            {
                ImGui::Text("%s", font.first.c_str()); 
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Font path: %s", font.first.c_str());
                }
            }
        }
        else if (asset_type == "Models")
        {
            for (const auto& model : NIKE_ASSETS_SERVICE->getLoadedModels())
            {
                ImGui::Text("%s", model.first.c_str());
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Model path: %s", model.first.c_str());
                }
            }
        }
        else if (asset_type == "Shaders")
        {
            for (const auto& shader : NIKE_ASSETS_SERVICE->getLoadedShaders())
            {
                ImGui::Text("%s", shader.first.c_str());
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Shader path: %s", shader.first.c_str());
                }
            }
        }
        // Add more asset types here if needed (e.g., models, shaders)

        ImGui::EndChild();
    }





    
}

