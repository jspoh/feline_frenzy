/*****************************************************************//**
 * \file   sLuaECSBinds.cpp
 * \brief  Lua ECS bindings
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (100%)
 * \date   March 2025
 * All content 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Lua/sLuaECSBinds.h"
#include "Components/cTransform.h"

namespace NIKE {

    void Lua::luaECSBinds(sol::state& lua_state) {

        lua_state.new_usertype<Transform::Transform>("Transform",
            "position", sol::property(
                [](Transform::Transform& t) { return t.position; },
                [](Transform::Transform& t, Vector2f val) { t.position = val; }
                ),
            "scale", sol::property(
                [](Transform::Transform& t) { return t.scale; },
                [](Transform::Transform& t, Vector2f val) { t.scale = val; }
                ),
            "SetScale", [](Transform::Transform& s, float x, float y) {
                s.scale.x = x;
                s.scale.y = y;
            },
            "rotation", &Transform::Transform::rotation,
            "use_screen_pos", &Transform::Transform::use_screen_pos
        );

        lua_state.new_usertype<Physics::Collider>("Collider",
            "Transform", sol::property(
                [](Physics::Collider& t) { return t.transform; },
                [](Physics::Collider& t, Transform::Transform val) { t.transform = val; }
                ),
            "Vertices", sol::property(
                [](Physics::Collider& t) { return t.vertices; },
                [](Physics::Collider& t, std::vector<Vector2f> val) { t.vertices = val; }
                )
        );

        lua_state.new_usertype<Audio::SFX>("SFX",
            "Play", sol::property(
                [](Audio::SFX& s) { return s.b_play_sfx; },
                [](Audio::SFX& s, bool val) { s.b_play_sfx = val; }
                ),
            "AudioID", sol::property(
                [](Audio::SFX& s) { return s.audio_id; },
                [](Audio::SFX& s, const std::string& val) { s.audio_id = val; }
                ),
            "ChannelGroupID", sol::property(
                [](Audio::SFX& s) { return s.channel_group_id; },
                [](Audio::SFX& s, const std::string& val) { s.channel_group_id = val; }
                ),
            "Volume", sol::property(
                [](Audio::SFX& s) { return s.volume; },
                [](Audio::SFX& s, float val) { s.volume = val; }
                ),
            "Pitch", sol::property(
                [](Audio::SFX& s) { return s.pitch; },
                [](Audio::SFX& s, float val) { s.pitch = val; }
                )
        );

        lua_state.new_usertype<Animation::Base>("Base",
            "AnimationMode", sol::property(
                [](Animation::Base& b) { return b.animation_mode; },
                [](Animation::Base& b, int val) { b.animation_mode = static_cast<Animation::Mode>(val); }
                ),
            "AnimationsToComplete", sol::property(
                [](Animation::Base& b) { return b.animations_to_complete; },
                [](Animation::Base& b, int val) { b.animations_to_complete = val; }
                ),
            "CompletedAnimations", sol::property(
                [](Animation::Base& b) { return b.completed_animations; },
                [](Animation::Base& b, int val) { b.completed_animations = val; }
                ),
            "PingPong", sol::property(
                [](Animation::Base& b) { return b.b_pingpong; },
                [](Animation::Base& b, bool val) { b.b_pingpong = val; }
                ),
            "Reverse", sol::property(
                [](Animation::Base& b) { return b.b_reverse; },
                [](Animation::Base& b, bool val) { b.b_reverse = val; }
                ),
            "FrameDuration", sol::property(
                [](Animation::Base& b) { return b.frame_duration; },
                [](Animation::Base& b, float val) { b.frame_duration = val; }
                ),
            "Timer", sol::property(
                [](Animation::Base& b) { return b.timer; }
                )
        );

        lua_state.new_usertype<Animation::Sprite>("Sprite",
            "SheetSize", sol::property(
                [](Animation::Sprite& s) { return s.sheet_size; }
                ),
            "StartIndex", sol::property(
                [](Animation::Sprite& s) { return s.start_index; },
                [](Animation::Sprite& s, Vector2i val) { s.start_index = val; }
                ),
            "EndIndex", sol::property(
                [](Animation::Sprite& s) { return s.end_index; },
                [](Animation::Sprite& s, Vector2i val) { s.end_index = val; }
                ),
            "CurrentIndex", sol::property(
                [](Animation::Sprite& s) { return s.curr_index; },
                [](Animation::Sprite& s, Vector2i val) { s.curr_index = val; }
                ),
            "SetStartIndex", [](Animation::Sprite& s, int x, int y) {
                s.start_index.x = x;
                s.start_index.y = y;
            },
            "SetEndIndex", [](Animation::Sprite& s, int x, int y) {
                s.end_index.x = x;
                s.end_index.y = y;
            },
            "SetCurrentIndex", [](Animation::Sprite& s, int x, int y) {
                s.curr_index.x = x;
                s.curr_index.y = y;
            }
        );

        lua_state.new_usertype<Render::Texture>("Texture",
            "TextureID", sol::property(
                [](Render::Texture& t) { return t.texture_id; },
                [](Render::Texture& t, const std::string& val) { t.texture_id = val; }
                ),
            "Color", sol::property(
                [](Render::Texture& t) { return t.color; },
                [](Render::Texture& t, Vector4f val) { t.color = val; }
                ),
            "FrameSize", sol::property(
                [](Render::Texture& t) { return t.frame_size; },
                [](Render::Texture& t, Vector2i val) { t.frame_size = val; }
                ),
            "FrameIndex", sol::property(
                [](Render::Texture& t) { return t.frame_index; },
                [](Render::Texture& t, Vector2i val) { t.frame_index = val; }
                ),
            "Blend", sol::property(
                [](Render::Texture& t) { return t.b_blend; },
                [](Render::Texture& t, bool val) { t.b_blend = val; }
                ),
            "Intensity", sol::property(
                [](Render::Texture& t) { return t.intensity; },
                [](Render::Texture& t, float val) { t.intensity = val; }
                ),
            "Stretch", sol::property(
                [](Render::Texture& t) { return t.b_stretch; },
                [](Render::Texture& t, bool val) { t.b_stretch = val; }
                ),
            "Flip", sol::property(
                [](Render::Texture& t) { return t.b_flip; },
                [](Render::Texture& t, Vector2b val) { t.b_flip = val; }
                ),
            "SetFrameSize", [](Render::Texture& t, int x, int y) {
                t.frame_size.x = x;
                t.frame_size.y = y;
            }
        );

        lua_state.new_usertype<Combat::Health>("Health",
            "lives", &Combat::Health::lives,
            "max_health", &Combat::Health::max_health,
            "health", &Combat::Health::health,
            "invulnerable_flag", &Combat::Health::invulnerable_flag,
            "taken_damage", &Combat::Health::taken_damage
        );

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
            NIKE_METADATA_SERVICE->destroyEntity(entity);
            });

        //Check if entity has component
        lua_state.set_function("HasComponent", [&](Entity::Type entity, const std::string& componentName) -> sol::optional<bool> {
            try {
                // Get the component type for the given name
                auto type = NIKE_ECS_MANAGER->getComponentType(componentName);

                // If the component type is valid (not -1)
                if (type != -1) {
                    bool hasComponent = NIKE_ECS_MANAGER->checkEntityComponent(entity, type);
                    return sol::make_optional(hasComponent);  // Return an optional with the result
                }
                else {
                    // Return an empty optional if the type is invalid
                    return sol::optional<bool>{};  // Explicitly return an empty optional
                }
            }
            catch ([[maybe_unused]] const std::runtime_error& e) {
                // Catch the error when the component is not registered or invalid
                NIKEE_ERROR("LUA: Invalid component name");
                // Return an empty optional, signaling an error in Lua without crashing
                return sol::optional<bool>{};  // Return an empty optional
            }
            });


        lua_state.set_function("GetComponent", [&](Entity::Type entity, std::string componentName) -> sol::object {
            auto type = NIKE_ECS_MANAGER->getComponentType(componentName);
            if (!NIKE_ECS_MANAGER->checkEntityComponent(entity, type)) {
                    return sol::nil;
            }

            // Use a template function to handle different component types
            if (componentName == "Physics::Collider") {
                auto component = std::static_pointer_cast<Physics::Collider>(
                    NIKE_ECS_MANAGER->getEntityComponent(entity, type)
                );
                return sol::make_object(lua_state, component);
            }
            // Use a template function to handle different component types
            if (componentName == "Audio::SFX") {
                auto component = std::static_pointer_cast<Audio::SFX>(
                    NIKE_ECS_MANAGER->getEntityComponent(entity, type)
                );
                return sol::make_object(lua_state, component);
            }
            // Use a template function to handle different component types
            if (componentName == "Transform::Transform") {
                    auto component = std::static_pointer_cast<Transform::Transform>(
                    NIKE_ECS_MANAGER->getEntityComponent(entity, type)
                );
                return sol::make_object(lua_state, component);
            }
            // Use a template function to handle different component types
            if (componentName == "Render::Texture") {
                auto component = std::static_pointer_cast<Render::Texture>(
                    NIKE_ECS_MANAGER->getEntityComponent(entity, type)
                );
                return sol::make_object(lua_state, component);
            }
            // Use a template function to handle different component types
            if (componentName == "Animation::Base") {
                auto component = std::static_pointer_cast<Animation::Base>(
                    NIKE_ECS_MANAGER->getEntityComponent(entity, type)
                );
                return sol::make_object(lua_state, component);
            }
            // Use a template function to handle different component types
            if (componentName == "Animation::Sprite") {
                auto component = std::static_pointer_cast<Animation::Sprite>(
                    NIKE_ECS_MANAGER->getEntityComponent(entity, type)
                );
                return sol::make_object(lua_state, component);
            }
            if (componentName == "Combat::Health") {
                auto component = std::static_pointer_cast<Combat::Health>(
                    NIKE_ECS_MANAGER->getEntityComponent(entity, type)
                );
                return sol::make_object(lua_state, component);
            }

            return sol::nil; // Component type not registered for Lua exposure
        });
    }

}