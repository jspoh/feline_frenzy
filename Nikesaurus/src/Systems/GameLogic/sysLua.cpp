/*****************************************************************//**
 * \file   sysLua.cpp
 * \brief  Lua manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Systems/GameLogic/sysLua.h"
#include "Systems/Physics/sysPhysics.h"

namespace NIKE {

    void Lua::System::registerBindings() {
    }

    Lua::System::System()
        : lua_state{ std::make_unique<sol::state>() } {}

    void Lua::System::init() {
        //Lua state init
        lua_state->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table);

        //Register function bindings
        registerBindings();
    }

    void Lua::System::loadScript(std::string const& script_id, std::string const& file_path) {
        sol::load_result script = lua_state->load_file(file_path);

        //Load failure
        if (!script.valid()) {
            sol::error err = script;
            NIKEE_CORE_WARN(err.what());
        }

        // Store the script for hot-reloading
        scripts[script_id].second = std::move(script);
    }

    void Lua::System::reloadScripts() {
        for (auto& script : scripts) {
            //Reload script file
            sol::load_result new_script = lua_state->load_file(script.second.first);

            //Load failure
            if (!new_script.valid()) {
                sol::error err = new_script;
                NIKEE_CORE_WARN(err.what());
            }

            //Update new script
            script.second.second = std::move(new_script);
            NIKEE_CORE_INFO("Script: " + script.first + " succesfully reloaded from: " + script.second.first);
        }
    }

    void Lua::System::executeScript(std::string const& script_id) {

        //Check if script is already loaded
        auto it = scripts.find(script_id);
        if (it == scripts.end()) {
            throw std::runtime_error("Script not loaded yet.");
        }

        //Execute script
        sol::protected_function_result execute = it->second.second();

        //Execute failure
        if (!execute.valid()) {
            sol::error err = execute;
            NIKEE_CORE_WARN(err.what());
        }
    }
}
