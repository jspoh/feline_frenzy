#include "Core/stdafx.h"
#include "Managers/Services/Imgui/sImgui.h"
#include "Core/Engine.h"

namespace NIKESAURUS
{
    void imguiInputUpdate()
    {
        ImGuiIO& io = ImGui::GetIO();
        // Seting imgui IO stuff
        io.DeltaTime = NIKE_WINDOWS_SERVICE->getDeltaTime();

        //// Pass in input to imgui
        //io.MousePos = ImVec2(NIKE_ENGINE.getService<Input::Service>()->getMousePos().x, NIKE_ENGINE.getService<Input::Service>()->getMousePos().y);
        //io.MouseDown[0] = NIKE_ENGINE.getService<Input::Service>()->isMousePressed(NIKE_MOUSE_BUTTON_1);
        //io.MouseDown[1] = NIKE_ENGINE.getService<Input::Service>()->isMousePressed(NIKE_MOUSE_BUTTON_2);

        //// Pass mouse scroll to imgui
        //io.MouseWheelH += NIKE_ENGINE.getService<Input::Service>()->getMouseScroll().x;
        //io.MouseWheel += NIKE_ENGINE.getService<Input::Service>()->getMouseScroll().y;


        io.DisplaySize = ImVec2(static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x),
            static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y));
    }

    // Check if the file has one of the valid extensions (png, jpg, wav)
    bool hasValidExtension(const std::filesystem::path& filePath) {
        std::string extension = filePath.extension().string();
        return (extension == ".png" || extension == ".jpg" || extension == ".wav" || extension == ".txt"
            || extension == ".scn" || extension == ".ogg" || extension == ".tex" || extension == ".ttf" || extension == ".frag" || extension == ".vert");
    }

    
}

