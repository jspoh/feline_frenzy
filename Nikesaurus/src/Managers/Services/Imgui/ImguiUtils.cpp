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
        return (extension == ".wav" || extension == ".ogg" );
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



    
}

