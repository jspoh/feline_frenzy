#include "Core/stdafx.h"
#include "Managers/Services/Imgui/sImgui.h"
#include "Core/Engine.h"

namespace NIKESAURUS
{
    void imguiInputUpdate()
    {
        ImGuiIO& io = ImGui::GetIO();
        // Seting imgui IO stuff
        io.DeltaTime = NIKEEngine.getService<Windows::Service>()->getDeltaTime();

        //// Pass in input to imgui
        //io.MousePos = ImVec2(NIKEEngine.getService<Input::Service>()->getMousePos().x, NIKEEngine.getService<Input::Service>()->getMousePos().y);
        //io.MouseDown[0] = NIKEEngine.getService<Input::Service>()->isMousePressed(NIKE_MOUSE_BUTTON_1);
        //io.MouseDown[1] = NIKEEngine.getService<Input::Service>()->isMousePressed(NIKE_MOUSE_BUTTON_2);

        //// Pass mouse scroll to imgui
        //io.MouseWheelH += NIKEEngine.getService<Input::Service>()->getMouseScroll().x;
        //io.MouseWheel += NIKEEngine.getService<Input::Service>()->getMouseScroll().y;


        io.DisplaySize = ImVec2(static_cast<float>(NIKEEngine.getService<Windows::Service>()->getWindow()->getWindowSize().x),
            static_cast<float>(NIKEEngine.getService<Windows::Service>()->getWindow()->getWindowSize().y));
    }

    // Check if the file has one of the valid extensions (png, jpg, wav)
    bool hasValidExtension(const std::filesystem::path& filePath) {
        std::string extension = filePath.extension().string();
        return (extension == ".png" || extension == ".jpg" || extension == ".wav" || extension == ".txt"
            || extension == ".scn" || extension == ".ogg" || extension == ".tex" || extension == ".ttf" || extension == ".frag" || extension == ".vert");
    }

    
}

