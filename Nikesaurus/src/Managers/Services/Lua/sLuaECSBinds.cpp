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

namespace NIKE {

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
            NIKE_METADATA_SERVICE->destroyEntity(entity);
            });

        //Register destroy entity
        lua_state.set_function("HasComponent", [&](Entity::Type entity, const std::string& componentName) -> bool {
            auto type = NIKE_ECS_MANAGER->getComponentType(componentName);
            return NIKE_ECS_MANAGER->checkEntityComponent(entity, type);
            });

    }

}