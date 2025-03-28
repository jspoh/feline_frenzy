/*****************************************************************//**
 * \file   sLuaCoreBinds.cpp
 * \brief  Lua bindings
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Lua/sLuaCoreBinds.h"

namespace NIKE {

    void Lua::luaKeyBinds(sol::state& lua_state) {

        //Lua key bindings
        lua_state.create_named_table("Key",
            "SPACE", NIKE_KEY_SPACE,
            "APOSTROPHE", NIKE_KEY_APOSTROPHE,
            "COMMA", NIKE_KEY_COMMA,
            "MINUS", NIKE_KEY_MINUS,
            "PERIOD", NIKE_KEY_PERIOD,
            "SLASH", NIKE_KEY_SLASH,
            "KEY_0", NIKE_KEY_0,
            "KEY_1", NIKE_KEY_1,
            "KEY_2", NIKE_KEY_2,
            "KEY_3", NIKE_KEY_3,
            "KEY_4", NIKE_KEY_4,
            "KEY_5", NIKE_KEY_5,
            "KEY_6", NIKE_KEY_6,
            "KEY_7", NIKE_KEY_7,
            "KEY_8", NIKE_KEY_8,
            "KEY_9", NIKE_KEY_9,
            "SEMICOLON", NIKE_KEY_SEMICOLON,
            "EQUAL", NIKE_KEY_EQUAL,
            "A", NIKE_KEY_A,
            "B", NIKE_KEY_B,
            "C", NIKE_KEY_C,
            "D", NIKE_KEY_D,
            "E", NIKE_KEY_E,
            "F", NIKE_KEY_F,
            "G", NIKE_KEY_G,
            "H", NIKE_KEY_H,
            "I", NIKE_KEY_I,
            "J", NIKE_KEY_J,
            "K", NIKE_KEY_K,
            "L", NIKE_KEY_L,
            "M", NIKE_KEY_M,
            "N", NIKE_KEY_N,
            "O", NIKE_KEY_O,
            "P", NIKE_KEY_P,
            "Q", NIKE_KEY_Q,
            "R", NIKE_KEY_R,
            "S", NIKE_KEY_S,
            "T", NIKE_KEY_T,
            "U", NIKE_KEY_U,
            "V", NIKE_KEY_V,
            "W", NIKE_KEY_W,
            "X", NIKE_KEY_X,
            "Y", NIKE_KEY_Y,
            "Z", NIKE_KEY_Z,
            "LEFT_BRACKET", NIKE_KEY_LEFT_BRACKET,
            "BACKSLASH", NIKE_KEY_BACKSLASH,
            "RIGHT_BRACKET", NIKE_KEY_RIGHT_BRACKET,
            "GRAVE_ACCENT", NIKE_KEY_GRAVE_ACCENT,
            "WORLD_1", NIKE_KEY_WORLD_1,
            "WORLD_2", NIKE_KEY_WORLD_2,
            "ESCAPE", NIKE_KEY_ESCAPE,
            "ENTER", NIKE_KEY_ENTER,
            "TAB", NIKE_KEY_TAB,
            "BACKSPACE", NIKE_KEY_BACKSPACE,
            "INSERT", NIKE_KEY_INSERT,
            "DELETE", NIKE_KEY_DELETE,
            "RIGHT", NIKE_KEY_RIGHT,
            "LEFT", NIKE_KEY_LEFT,
            "DOWN", NIKE_KEY_DOWN,
            "UP", NIKE_KEY_UP,
            "PAGE_UP", NIKE_KEY_PAGE_UP,
            "PAGE_DOWN", NIKE_KEY_PAGE_DOWN,
            "HOME", NIKE_KEY_HOME,
            "END", NIKE_KEY_END,
            "CAPS_LOCK", NIKE_KEY_CAPS_LOCK,
            "SCROLL_LOCK", NIKE_KEY_SCROLL_LOCK,
            "NUM_LOCK", NIKE_KEY_NUM_LOCK,
            "PRINT_SCREEN", NIKE_KEY_PRINT_SCREEN,
            "PAUSE", NIKE_KEY_PAUSE,
            "F1", NIKE_KEY_F1,
            "F2", NIKE_KEY_F2,
            "F3", NIKE_KEY_F3,
            "F4", NIKE_KEY_F4,
            "F5", NIKE_KEY_F5,
            "F6", NIKE_KEY_F6,
            "F7", NIKE_KEY_F7,
            "F8", NIKE_KEY_F8,
            "F9", NIKE_KEY_F9,
            "F10", NIKE_KEY_F10,
            "F11", NIKE_KEY_F11,
            "F12", NIKE_KEY_F12,
            "F13", NIKE_KEY_F13,
            "F14", NIKE_KEY_F14,
            "F15", NIKE_KEY_F15,
            "F16", NIKE_KEY_F16,
            "F17", NIKE_KEY_F17,
            "F18", NIKE_KEY_F18,
            "F19", NIKE_KEY_F19,
            "F20", NIKE_KEY_F20,
            "F21", NIKE_KEY_F21,
            "F22", NIKE_KEY_F22,
            "F23", NIKE_KEY_F23,
            "F24", NIKE_KEY_F24,
            "F25", NIKE_KEY_F25,
            "KP_0", NIKE_KEY_KP_0,
            "KP_1", NIKE_KEY_KP_1,
            "KP_2", NIKE_KEY_KP_2,
            "KP_3", NIKE_KEY_KP_3,
            "KP_4", NIKE_KEY_KP_4,
            "KP_5", NIKE_KEY_KP_5,
            "KP_6", NIKE_KEY_KP_6,
            "KP_7", NIKE_KEY_KP_7,
            "KP_8", NIKE_KEY_KP_8,
            "KP_9", NIKE_KEY_KP_9,
            "KP_DECIMAL", NIKE_KEY_KP_DECIMAL,
            "KP_DIVIDE", NIKE_KEY_KP_DIVIDE,
            "KP_MULTIPLY", NIKE_KEY_KP_MULTIPLY,
            "KP_SUBTRACT", NIKE_KEY_KP_SUBTRACT,
            "KP_ADD", NIKE_KEY_KP_ADD,
            "KP_ENTER", NIKE_KEY_KP_ENTER,
            "KP_EQUAL", NIKE_KEY_KP_EQUAL,
            "LEFT_SHIFT", NIKE_KEY_LEFT_SHIFT,
            "LEFT_CONTROL", NIKE_KEY_LEFT_CONTROL,
            "LEFT_ALT", NIKE_KEY_LEFT_ALT,
            "LEFT_SUPER", NIKE_KEY_LEFT_SUPER,
            "RIGHT_SHIFT", NIKE_KEY_RIGHT_SHIFT,
            "RIGHT_CONTROL", NIKE_KEY_RIGHT_CONTROL,
            "RIGHT_ALT", NIKE_KEY_RIGHT_ALT,
            "RIGHT_SUPER", NIKE_KEY_RIGHT_SUPER,
            "MENU", NIKE_KEY_MENU,
            "MOUSE_LEFT", NIKE_MOUSE_BUTTON_LEFT,
            "MOUSE_RIGHT", NIKE_MOUSE_BUTTON_RIGHT,
            "MOUSE_MIDDLE", NIKE_MOUSE_BUTTON_MIDDLE
        );
    }

    void Lua::luaBasicBinds(sol::state& lua_state) {
        //Bind Cout for lua
        lua_state.set_function("cout", sol::overload(
            [](const std::string& msg) {
                cout << "[Lua] " << msg << endl;
            },
            [](int value) {
                cout << "[Lua] " << value << endl;
            },
            [](bool value) {
                cout << "[Lua] " << value << endl;
            },
            [](float value) {
                cout << "[Lua] " << value << endl;
            }
        ));

        //Define the noop function in Lua
        lua_state.script(R"(
            function noop()
                -- No operation
            end
        )");

        //New usertype vector2f
        lua_state.new_usertype<Vector2f>("Vector2f",
            "x", sol::property(
                [](Vector2f& t) { return t.x; },
                [](Vector2f& t, float val) { t.x = val; }
                ),
            "y", sol::property(
                [](Vector2f& t) { return t.y; },
                [](Vector2f& t, float val) { t.y = val; }
                )
        );

        //New usertype vector2i
        lua_state.new_usertype<Vector2i>("Vector2i",
            "x", sol::property(
                [](Vector2i& t) { return t.x; },
                [](Vector2i& t, int val) { t.x = val; }
                ),
            "y", sol::property(
                [](Vector2i& t) { return t.y; },
                [](Vector2i& t, int val) { t.y = val; }
                )
        );

        //New usertype vector2f
        lua_state.new_usertype<Vector4f>("Vector4f",
            sol::constructors<Vector4f(), Vector4f(float, float, float, float)>(),
            "x", &Vector4f::x,
            "y", &Vector4f::y,
            "z", &Vector4f::z,
            "w", &Vector4f::w,
            "r", &Vector4f::r,
            "g", &Vector4f::g,
            "b", &Vector4f::b,
            "a", &Vector4f::a
        );

        //New usertype vector2i
        lua_state.new_usertype<Vector4i>("Vector4i",
            sol::constructors<Vector4i(), Vector4i(int, int, int, int)>(),
            "x", &Vector4i::x,
            "y", &Vector4i::y,
            "z", &Vector4i::z,
            "w", &Vector4i::w,
            "r", &Vector4i::r,
            "g", &Vector4i::g,
            "b", &Vector4i::b,
            "a", &Vector4i::a
        );

        // atan2 function (lua math.atan2 apparantly doesn't exist)
        lua_state.set_function("atan2", [](float y, float x) {
            return std::atan2(y, x);  // Use the C++ std::atan2 function
            });
    }

    void Lua::luaInputBinds(sol::state& lua_state) {
        lua_state.set_function("IsKeyPressed", [](int key) -> bool {
            return NIKE_INPUT_SERVICE->isKeyPressed(key);
            });

        lua_state.set_function("IsKeyTriggered", [](int key) -> bool {
            return NIKE_INPUT_SERVICE->isKeyTriggered(key);
            });

        lua_state.set_function("IsKeyReleased", [](int key) -> bool {
            return NIKE_INPUT_SERVICE->isKeyReleased(key);
            });

        lua_state.set_function("IsMousePressed", [](int key) -> bool {
            return NIKE_INPUT_SERVICE->isMousePressed(key);
            });

        lua_state.set_function("IsMouseTriggered", [](int key) -> bool {
            return NIKE_INPUT_SERVICE->isMouseTriggered(key);
            });

        lua_state.set_function("IsMouseReleased", [](int key) -> bool {
            return NIKE_INPUT_SERVICE->isMouseReleased(key);
            });

        lua_state.set_function("WorldMousePos", []() {
            return NIKE_INPUT_SERVICE->getMouseWorldPos();
            });

        lua_state.set_function("WindowMousePos", []() {
            return NIKE_INPUT_SERVICE->getMouseWindowPos();
            });
    }

    void Lua::luaSceneBinds(sol::state& lua_state) {

        lua_state.set_function("ChangeScene", [&](std::string const& scene) {
            const float dt = NIKE_WINDOWS_SERVICE->getFixedDeltaTime();

            constexpr float fade_duration = 2.f;
            constexpr float max_alpha = 255.0f;
            float elapsed_time = 0.0f;
            while (elapsed_time < fade_duration) {
                // Fade by steps
                float alpha_change = (max_alpha / fade_duration) * dt;
                auto const& overlay_entities = NIKE_METADATA_SERVICE->getEntitiesByTag("overlay");
                for (const auto& entity : overlay_entities)
                {
                    // Change entitiy's alpha value for fade
                    auto e_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);
                    // Why is the comp here nullopt??
                    if (e_shape_comp.has_value())
                    {
                        e_shape_comp.value().get().color.a += alpha_change;
                        if (e_shape_comp.value().get().color.a > max_alpha) {
                            e_shape_comp.value().get().color.a = max_alpha;
                        }
                    }
                }
                // Use delta time for fading
                elapsed_time += dt;
            }

            NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, scene));
            });


        lua_state.set_function("RestartScene", [&]() {
            NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::RESTART, ""));
            });

        lua_state.set_function("PreviousScene", [&]() {
            constexpr float fade_duration = 2.f;
            constexpr float max_alpha = 255.0f;
            float elapsed_time = 0.0f;
            while (elapsed_time < fade_duration) {
                // Fade by steps
                float alpha_change = (max_alpha / fade_duration) * NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
                auto const& overlay_entities = NIKE_METADATA_SERVICE->getEntitiesByTag("overlay");
                for (const auto& entity : overlay_entities)
                {
                    // Change entitiy's alpha value for fade
                    auto e_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);
                    if (e_shape_comp.has_value())
                    {
                        e_shape_comp.value().get().color.a += alpha_change;
                        if (e_shape_comp.value().get().color.a > max_alpha) {
                            e_shape_comp.value().get().color.a = max_alpha;
                        }
                    }
                }
                // Use delta time for fading
                elapsed_time += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
            }
            NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::PREVIOUS, ""));
            });

        lua_state.set_function("CloseScene", [&]() {
            NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CLOSE, ""));
            });

        lua_state.set_function("QuitScene", [&]() {
            NIKE_WINDOWS_SERVICE->getWindow()->terminate();
            });

    }

}
