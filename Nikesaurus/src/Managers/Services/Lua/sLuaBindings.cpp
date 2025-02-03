/*****************************************************************//**
 * \file   sLuaBindings.cpp
 * \brief  Lua bindings
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
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
            sol::constructors<Vector2f(), Vector2f(float, float)>(),
            "x", &Vector2f::x,
            "y", &Vector2f::y
        );

        //New usertype vector2i
        lua_state.new_usertype<Vector2i>("Vector2i",
            sol::constructors<Vector2i(), Vector2i(int, int)>(),
            "x", &Vector2i::x,
            "y", &Vector2i::y
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

        lua_state.set_function("QuitScene", [&]() {
            NIKE_WINDOWS_SERVICE->getWindow()->terminate();
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

    void Lua::luaGameBinds(sol::state& lua_state) {

        //Apply force to entities
        lua_state.set_function("ApplyForce", [&](Entity::Type entity, float x, float y) {
            auto e_trans_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_trans_comp.has_value()) {
                e_trans_comp.value().get().force.x = x;
                e_trans_comp.value().get().force.y = y;
            }
            });

        //Change animation start & end
        lua_state.set_function("AnimationStart", [&](Entity::Type entity, int start_x, int start_y) {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);
            if (e_animate_comp.has_value()) {
                static Vector2i prev_start = e_animate_comp.value().get().start_index;

                if (prev_start != Vector2i(start_x, start_y)) {
                    e_animate_comp.value().get().start_index.x = start_x;
                    e_animate_comp.value().get().start_index.y = start_y;
                    prev_start = e_animate_comp.value().get().start_index;

                    //Restart animation
                    auto e_base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
                    if (e_base_comp.has_value()) {
                        e_base_comp.value().get().animation_mode = Animation::Mode::RESTART;
                    }
                }
            }
            });

        //Change animation start & end
        lua_state.set_function("AnimationEnd", [&](Entity::Type entity, int end_x, int end_y) {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);
            if (e_animate_comp.has_value()) {
                static Vector2i prev_end = e_animate_comp.value().get().end_index;

                if (prev_end != Vector2i(end_x, end_y)) {
                    e_animate_comp.value().get().end_index.x = end_x;
                    e_animate_comp.value().get().end_index.y = end_y;
                    prev_end = e_animate_comp.value().get().end_index;

                    //Restart animation
                    auto e_base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
                    if (e_base_comp.has_value()) {
                        e_base_comp.value().get().animation_mode = Animation::Mode::RESTART;
                    }
                }
            }
            });

        //Get Last Direction (COMPONENT TO BE MOVED ELSE WHERE)
        lua_state.set_function("AnimationCompleted", [&](Entity::Type entity) -> sol::optional<int> {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent < Animation::Base > (entity);
            if (e_animate_comp.has_value()) {
                return e_animate_comp.value().get().completed_animations;
            }
            else {
                return sol::optional<int>();
            }
            });

        //Flip texture
        lua_state.set_function("FlipX", [&](Entity::Type entity, bool yes_or_no) {
            auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
            if (e_texture_comp.has_value()) {
                e_texture_comp.value().get().b_flip.x = yes_or_no;
            }
            });

        lua_state.set_function("FlipY", [&](Entity::Type entity, bool yes_or_no) {
            auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
            if (e_texture_comp.has_value()) {
                e_texture_comp.value().get().b_flip.y = yes_or_no;
            }
            });

        //Get Velocity
        lua_state.set_function("Velocity", [&](Entity::Type entity) -> sol::optional<Vector2f> {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_animate_comp.has_value()) {
                return e_animate_comp.value().get().velocity;
            }
            else {
                return sol::optional<Vector2f>();
            }
            });

        //Get Net Velocity
        lua_state.set_function("NetVelocity", [&](Entity::Type entity) -> sol::optional<float> {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_animate_comp.has_value()) {
                return e_animate_comp.value().get().velocity.length();
            }
            else {
                return sol::optional<float>();
            }
            });

        //Apply force to entities (COMPONENT TO BE MOVED ELSE WHERE)
        lua_state.set_function("SetLastDirection", [&](Entity::Type entity, int dir) {
            auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_physics_comp.has_value()) {
                e_physics_comp.value().get().last_direction = dir;
            }
            });

        //Get Last Direction (COMPONENT TO BE MOVED ELSE WHERE)
        lua_state.set_function("LastDirection", [&](Entity::Type entity) -> sol::optional<int> {
            auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_physics_comp.has_value()) {
                return e_physics_comp.value().get().last_direction;
            }
            else {
                return sol::optional<int>();
            }
            });

        lua_state.set_function("SetState", [&](Entity::Type entity, std::string state) {
            auto e_state_comp = NIKE_ECS_MANAGER->getEntityComponent<State::State>(entity);
            if (e_state_comp.has_value()) {
                e_state_comp.value().get().state_id = state;
            }
            });

        lua_state.set_function("State", [&](Entity::Type entity) -> sol::optional<std::string> {
            auto e_state_comp = NIKE_ECS_MANAGER->getEntityComponent<State::State>(entity);
            if (e_state_comp.has_value()) {
                return e_state_comp.value().get().state_id;
            }
            else {
                return sol::optional<std::string>();
            }
            });

        //Get SFX
        lua_state.set_function("PlaySFX", [&](Entity::Type entity, bool play_or_stop) {
            auto e_sfx_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
            if (e_sfx_comp.has_value()) {
                auto& e_sfx = e_sfx_comp.value().get();

                //Check if group exists
                auto group = NIKE_AUDIO_SERVICE->getChannelGroup(e_sfx.channel_group_id);
                if (!group) {
                    e_sfx.b_play_sfx = play_or_stop;
                    return;
                }
                else {
                    //Play sound
                    if (play_or_stop && !group->isPlaying()) {
                        e_sfx.b_play_sfx = play_or_stop;
                    }
                }

                //stop sfx
                if (!play_or_stop) {
                    group->stop();
                }
            }
            });

        //Fire Bullet
        lua_state.set_function("FireBullet", [&](Entity::Type entity) {

            Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity();

            // Player Element Type
            auto player_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(entity);
            if (player_element_comp.has_value()) {
                // Shoot elemental bullet
                NIKE_SERIALIZE_SERVICE->loadEntityFromFile(bullet_entity, NIKE_ASSETS_SERVICE->getAssetPath(Element::playerBullet[static_cast<int>(player_element_comp.value().get().element)]).string());
            }
            else {
                // Missing Element Comp
                NIKEE_CORE_WARN("PLAYER missing Elemental Component");
                NIKE_SERIALIZE_SERVICE->loadEntityFromFile(bullet_entity, NIKE_ASSETS_SERVICE->getAssetPath("bullet.prefab").string());
            }

            // Set/Teleport entity position
            lua_state.set_function("setPosition", [&](Entity::Type entity, float x, float y) {
                auto transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
                if (transform.has_value()) {
                    transform.value().get().position = { x, y };
                }
                });

#ifndef NDEBUG
            //auto data = NIKE_LVLEDITOR_SERVICE->getEntityMetaData(bullet_entity);
            //data.prefab_id = "bullet.prefab";
            //NIKE_LVLEDITOR_SERVICE->setEntityMetaData(bullet_entity, data);
#endif

            //Player position
            auto player_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
            if (!player_transform_comp.has_value()) {
                return;
            }

            //Get player transform
            auto const& player_transform = player_transform_comp.value().get();

            //Calculate direction vector (mouse - player)
            float direction = atan2((-(NIKE_INPUT_SERVICE->getMouseWorldPos().y) + player_transform.position.y), (NIKE_INPUT_SERVICE->getMouseWorldPos().x - player_transform.position.x));
            Vector2f bull_direction = { cosf(direction), -sinf(direction) };

            //Normalize direction
            if (bull_direction.length() != 0) {
                bull_direction.x /= bull_direction.length();
                bull_direction.y /= bull_direction.length();
            }

            //Bullet Speed
            int constexpr BULLET_SPEED = 100;

            //Calculate bullet force vector
            Vector2f bullet_force = bull_direction * BULLET_SPEED;

            // Apply force to the bullet
            auto bullet_dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(bullet_entity);
            if (bullet_dynamics.has_value()) {
                bullet_dynamics.value().get().force = bullet_force;
            }

            //Set initial bullet position to player's position
            auto bullet_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(bullet_entity);
            if (bullet_transform.has_value()) {
                bullet_transform.value().get().position = player_transform.position + (bull_direction.normalize() * (player_transform.scale.length() * 0.75f));
            }

            //Set player bullet SFX
            auto bullet_sfx = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(bullet_entity);
            if (bullet_sfx.has_value()) {
                bullet_sfx.value().get().b_play_sfx = true;
            }
            });

        //Spawn enemy function
        lua_state.set_function("Spawn Enemy", [&](float x, float y) {
            Entity::Type entity = NIKE_ECS_MANAGER->createEntity();
            NIKE_SERIALIZE_SERVICE->loadEntityFromFile(entity, NIKE_ASSETS_SERVICE->getAssetPath("Enemy.prefab").string());
            auto e_trans_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
            if (e_trans_comp.has_value()) {
                e_trans_comp.value().get().position.x = x;
                e_trans_comp.value().get().position.y = y;
            }
            });

        //Play audio
        lua_state.set_function("PlayAudio", [&](std::string const& audio_id) {
            NIKE_AUDIO_SERVICE->playAudio(audio_id, "", "SFX", 1.f, 1.f, false, false);
            });

        // Set/Teleport entity
        lua_state.set_function("SetPosition", [&](Entity::Type entity, float x, float y) {
            auto transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
            if (transform.has_value()) {
                transform.value().get().position = { x, y };
            }
            });

        // God mode toggle
        lua_state.set_function("SetGodMode", [&](Entity::Type entity, bool enable) {
            auto health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
            if (health_comp) {
                health_comp.value().get().invulnerableFlag = enable;
                if (health_comp.value().get().invulnerableFlag) {
                    cout << "Player god mode enabled" << endl;
                }
                else {
                    cout << "Player god mode disabled" << endl;
                }
            }
            });

        // High Damage toggle
        lua_state.set_function("SetHighDamage", [&](Entity::Type entity, bool enable) {
            auto damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Damage>(entity);
            if (damage_comp) {
                if (enable) {
                    damage_comp.value().get().damage = 9999.0f; // High damage
                    cout << "Damage set max" << endl;
                }
                else {
                    damage_comp.value().get().damage = 1.0f; // Default damage
                    cout << "Damage set default 1" << endl;
                }
            }
            });

        //Path finding
        lua_state.set_function("PathFind", [&](Entity::Type entity, int x_index, int y_index, float speed) {

            //Acceptable offset per cell
            const float cell_offset = 10.0f;

            //Get transform of entity for position mapping
            auto transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
            if (transform.has_value()) {

                //Entity transform
                auto& e_transform = transform.value().get();

                //Get index of entity as the starting position
                auto start = NIKE_MAP_SERVICE->getCellIndexFromCords(e_transform.position);

                //Get cell to travel to
                if (start.has_value()) {

                    //Get start index
                    auto start_index = start.value();

                    //Check if path has been generated or if destination cell has changed
                    if (!NIKE_MAP_SERVICE->checkPath(entity) ||

                        //Condition if changes to grid blocked has been made
                        NIKE_MAP_SERVICE->checkGridChanged() ||

                        //Check if target got shifted
                        (NIKE_MAP_SERVICE->getPath(entity).goal.index != Vector2i(x_index, y_index)) ||

                        //Check if entity got shifted
                        (!NIKE_MAP_SERVICE->getPath(entity).path.empty() &&
                            (std::abs(NIKE_MAP_SERVICE->getPath(entity).path.front().index.x - start_index.x) > 1 ||
                                std::abs(NIKE_MAP_SERVICE->getPath(entity).path.front().index.y - start_index.y) > 1)) ||

                        //Check if path is finished & entity got shifted
                        (NIKE_MAP_SERVICE->getPath(entity).b_finished && start_index != NIKE_MAP_SERVICE->getPath(entity).end.index)

                        ) {

                        //Search for path
                        NIKE_MAP_SERVICE->findPath(entity, start_index, Vector2i(x_index, y_index));
                    }

                    //Get path 
                    auto path = NIKE_MAP_SERVICE->getPath(entity);

                    //Check if there are cells left in path
                    if (!path.path.empty()) {

                        //Get next cell
                        auto const& next_cell = path.path.front();

                        //Check if entity has arrived near destination
                        if ((next_cell.position - e_transform.position).length() > cell_offset) {

                            //Direction of next cell
                            float dir = atan2((next_cell.position.y - e_transform.position.y), (next_cell.position.x - e_transform.position.x));

                            //Apply force to entity
                            auto dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
                            if (dynamics.has_value()) {
                                dynamics.value().get().force = { cos(dir) * speed, sin(dir) * speed };
                            }
                        }
                        else {
                            path.path.pop_front();
                        }
                    }
                    else {

                        //Marked path as finished
                        path.b_finished = true;
                    }
                }
            }
            });

    }
}
