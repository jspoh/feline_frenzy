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
            "rotation", &Transform::Transform::rotation,
            "use_screen_pos", &Transform::Transform::use_screen_pos
        );

        lua_state.new_usertype<Physics::Collider>("Collider",
            "Transform", sol::property(
                [](Physics::Collider& t) { return t.transform; },
                [](Physics::Collider& t, Physics::Collider val) { t.transform = val.transform; }
                ),
            "Vertices", sol::property(
                [](Physics::Collider& t) { return t.vertices; },
                [](Physics::Collider& t, std::vector<Vector2f> val) { t.vertices = val; }
                )
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
            if (componentName == "Transform::Transform") {
                    auto component = std::static_pointer_cast<Transform::Transform>(
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