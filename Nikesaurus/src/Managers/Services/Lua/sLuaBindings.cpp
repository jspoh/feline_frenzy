/*****************************************************************//**
 * \file   sLuaBindings.cpp
 * \brief  Lua bindings
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Lua/sLuaBindings.h"

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
            "MOUSE_BUTTON_1", NIKE_MOUSE_BUTTON_1,
            "MOUSE_BUTTON_2", NIKE_MOUSE_BUTTON_2,
            "MOUSE_BUTTON_3", NIKE_MOUSE_BUTTON_3,
            "MOUSE_BUTTON_4", NIKE_MOUSE_BUTTON_4,
            "MOUSE_BUTTON_5", NIKE_MOUSE_BUTTON_5,
            "MOUSE_BUTTON_6", NIKE_MOUSE_BUTTON_6,
            "MOUSE_BUTTON_7", NIKE_MOUSE_BUTTON_7,
            "MOUSE_BUTTON_8", NIKE_MOUSE_BUTTON_8,
            "MOUSE_BUTTON_LEFT", NIKE_MOUSE_BUTTON_LEFT,
            "MOUSE_BUTTON_RIGHT", NIKE_MOUSE_BUTTON_RIGHT,
            "MOUSE_BUTTON_MIDDLE", NIKE_MOUSE_BUTTON_MIDDLE
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
            }
        ));

        //Define the noop function in Lua
        lua_state.script(R"(
            function noop()
                -- No operation
            end
        )");
    }

    template<typename T>
    void Lua::registerVector2(sol::state& lua_state, const std::string& type_name) {
        using Vector2Type = Vector2<T>;

        auto math = lua_state["Math"].get_or_create<sol::table>();

        math.new_usertype<Vector2Type>(type_name.c_str(),
            sol::constructors<Vector2Type(), Vector2Type(T, T)>(),
            "x", &Vector2Type::x,
            "y", &Vector2Type::y,
            "length", &Vector2Type::length,
            "normalize", &Vector2Type::normalize,
            "dot", &Vector2Type::dot,
            sol::meta_function::addition, &Vector2Type::operator+,
            sol::meta_function::subtraction, &Vector2Type::operator-,
            sol::meta_function::multiplication, [](Vector2Type& lhs, T scalar) { return lhs * scalar; },
            sol::meta_function::division, & Vector2Type::operator/
        );
    }

    template<typename T>
    void Lua::registerVector4(sol::state& lua_state, const std::string& type_name) {
        using Vector4Type = Vector4<T>;

        auto math = lua_state["Math"].get_or_create<sol::table>();

        math.new_usertype<Vector4Type>(type_name.c_str(),
            sol::constructors<Vector4Type(), Vector4Type(T, T, T, T)>(),
            "x", &Vector4Type::x,
            "y", &Vector4Type::y,
            "z", &Vector4Type::z,
            "w", &Vector4Type::w,
            "r", &Vector4Type::r,
            "g", &Vector4Type::g,
            "b", &Vector4Type::b,
            "a", &Vector4Type::a,
            "length", &Vector4Type::length,
            "normalize", &Vector4Type::normalize,
            "dot", &Vector4Type::dot,
            sol::meta_function::addition, &Vector4Type::operator+,
            sol::meta_function::subtraction, &Vector4Type::operator-,
            sol::meta_function::multiplication, [](Vector4Type& lhs, T scalar) { return lhs * scalar; },
            sol::meta_function::division, & Vector4Type::operator/
        );
    }

    void Lua::luaMathBinds(sol::state& lua_state) {

        registerVector2<float>(lua_state, "Vector2f");
        registerVector2<int>(lua_state, "Vector2i");

        registerVector4<float>(lua_state, "Vector4f");
        registerVector4<int>(lua_state, "Vector4i");
    }

    void Lua::luaSceneBinds(sol::state& lua_state) {

        lua_state.set_function("ChangeScene", [&](std::string const& scene) {
            NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, scene));
            });

        lua_state.set_function("RestartScene", [&]() {
            NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::RESTART, ""));
            });

        lua_state.set_function("PreviousScene", [&]() {
            NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::PREVIOUS, ""));
            });

        lua_state.set_function("CloseScene", [&]() {
            NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CLOSE, ""));
            });
    }

    void Lua::luaECSBinds(sol::state& lua_state) {

        //Register create entity
        lua_state.set_function("NewEntity", [&]() -> Entity::Type {
            return NIKE_ECS_MANAGER->createEntity();
            });

        //Register destroy entities
        lua_state.set_function("KillAllEntities", [&]() {
            NIKE_ECS_MANAGER->destroyAllEntities();
            });

        //Register destroy entity
        lua_state.set_function("KillEntity", [&](Entity::Type entity) {
            NIKE_ECS_MANAGER->destroyEntity(entity);
            });

    }
}
