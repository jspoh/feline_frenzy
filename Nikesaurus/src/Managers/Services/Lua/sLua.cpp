/*****************************************************************//**
 * \file   sLua.cpp
 * \brief  Lua manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Lua/sLua.h"
#include "Managers/Services/Lua/sLuaCoreBinds.h"
#include "Managers/Services/Lua/sLuaECSBinds.h"
#include "Managers/Services/Lua/sLuaGameBinds.h"

namespace NIKE {

    std::shared_ptr<sol::load_result> Lua::Service::getLuaAssset(std::string const& script_id) const {
        //Get script table from asset service
       return NIKE_ASSETS_SERVICE->getAsset<sol::load_result>(script_id);
    }

    void Lua::Service::uncacheLuaAsset(std::string const& script_id) const {
        NIKE_ASSETS_SERVICE->uncacheAsset(script_id);
    }

    void Lua::Service::init() {
        lua_state = std::make_unique<sol::state>();

        //Lua state init
        lua_state->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::io, sol::lib::package);

        //Register all bindings here
        luaBasicBinds(*lua_state);
        luaKeyBinds(*lua_state);
        luaInputBinds(*lua_state);
        luaSceneBinds(*lua_state);
        luaECSBinds(*lua_state);
        luaGameBinds(*lua_state);

        // Ensure "package.path" exists and is a string before modifying it
        sol::object path_obj = (*lua_state)["package"]["path"];

        if (path_obj.is<std::string>()) {
            std::string current_path = path_obj.as<std::string>();
            (*lua_state)["package"]["path"] = current_path + ";assets/Scripts/?.lua";
        }
        else {
            (*lua_state)["package"]["path"] = "assets/Scripts/?.lua";
        }

        //Get all lua global functions
        sol::table globals = lua_state->globals();
        for (const auto& pair : globals) {
            sol::object key = pair.first;
            sol::object value = pair.second;

            if (value.is<sol::function>()) {
                lua_global_funcs.insert(key.as<std::string>());
            }
        }
    }

    void Lua::Service::convertSchemaToConfig(sol::table const& schema, std::unordered_map<std::string, LuaValue>& configs) {

        for (auto& [key_obj, val_obj] : schema) {
            std::string key = key_obj.as<std::string>();

            if (val_obj.is<std::string>()) {
                configs[key] = val_obj.as<std::string>();
            }
            else if (val_obj.is<bool>()) {
                configs[key] = val_obj.as<bool>();
            }
            else if (val_obj.is<int>()) {
                configs[key] = val_obj.as<int>();
            }
            else if (val_obj.is<float>()) {
                configs[key] = val_obj.as<float>();
            }
        }
    }

    void Lua::Service::convertConfigToSchema(std::unordered_map<std::string, LuaValue> const& configs, sol::table& schema) {
        for (const auto& [key, value] : configs) {
            std::visit([&, copied_key = key](auto&& v) {
                schema[copied_key] = v;
            }, value);
        }
    }

    std::vector<std::string> Lua::Service::getTableKeys(sol::table const& table) {
        if (!table.valid()) return std::vector<std::string>();

        std::vector<std::string> keys;
        for (auto& [key_obj, val_obj] : table) {
            std::string key = key_obj.as<std::string>();
            keys.push_back(key);
        }
        return keys;
    }

    Lua::ScriptObj Lua::Service::loadScript(std::filesystem::path const& path) {

        //Create empty script obj
        ScriptObj obj;

        //Get lua script
        obj.path = path;

        //Load script file
        sol::load_result chunk = lua_state->load_file(obj.path.string());
        if (!chunk.valid()) {
            sol::error err = chunk;
            NIKEE_CORE_WARN(err.what());
            return Lua::ScriptObj();
        }

        // Script executed here (top-level code runs)
        sol::protected_function_result result = chunk();
        if (!result.valid()) {
            sol::error err = result;
            NIKEE_CORE_WARN(err.what());
            return Lua::ScriptObj();
        }

        //Read schema
        sol::object schema_obj = (*lua_state)["schema"];
        sol::table schema;
        if (schema_obj.valid() && schema_obj.get_type() == sol::type::table) {
            schema = schema_obj.as<sol::table>();

            //Convert schema to conifgs
            convertSchemaToConfig(schema, obj.configs);
            (*lua_state)["schema"] = sol::nil;
        }

        //Return lua script object
        return obj;
    }

    void Lua::Service::initScript(std::string const& script_id, std::unordered_map<std::string, LuaValue> const& configs, sol::table& script_instance, bool& init) {

        //Get script object
        auto cached_script = NIKE_ASSETS_SERVICE->getAsset<Lua::ScriptObj>(script_id);

        //Load script file
        sol::load_result chunk = lua_state->load_file(cached_script->path.string());
        if (!chunk.valid()) {
            NIKEE_CORE_WARN("Error loading script! Probably due to syntax error");
            return;
        }

        // Script executed here (top-level code runs)
        sol::protected_function_result chunck_result = chunk();
        if (!chunck_result.valid()) {
            NIKEE_CORE_WARN("Error loading script! Probably due to syntax error");
            return;
        }

        //Create config table
        sol::table config_table = lua_state->create_table();
        convertConfigToSchema(configs, config_table);

        //Obtain sol table from script
        if (chunck_result.get_type() == sol::type::function) {

            //Get protecte function from result
            sol::protected_function func = chunck_result;
            sol::protected_function_result result = func(config_table);
            if (result.valid()) {

                //Obtain object and double check
                sol::object obj = result;
                if (obj.get_type() == sol::type::table) {

                    //Extract sol table
                    script_instance = result.get<sol::table>();

                    //Set flagged to successful
                    init = true;
                }
            }
            else {
                NIKEE_CORE_WARN("No lua table stored under script instances, there might be errors with the constructor");
            }
        }
        else {
            NIKEE_CORE_WARN("Result does not return a function for creating lua object! Invalid script!");
        }
    }

    std::vector<std::string> Lua::Service::getScriptFunctions(std::string const& virtual_path) {

        //Resolve virtual path
        auto path = NIKE_PATH_SERVICE->resolvePath(virtual_path);

        //Open script file
        std::ifstream script_file(path);
        if (!script_file.is_open()) {
            NIKEE_CORE_WARN("Unable to open Lua script file: {}", path.string());
            return {};
        }

        //Read script into memory
        std::string script_content((std::istreambuf_iterator<char>(script_file)),
            std::istreambuf_iterator<char>());
        script_file.close();

        //Regex pattern for Lua function definitions
        std::regex function_pattern(R"(function\s+([a-zA-Z_][a-zA-Z0-9_]*)(?::([a-zA-Z_][a-zA-Z0-9_]*))?\s*\()");
        std::smatch match;

        //Vector to store extracted functions
        std::vector<std::string> functions;

        //Search for all matches
        auto start = script_content.cbegin();
        auto end = script_content.cend();
        while (std::regex_search(start, end, match, function_pattern)) {
            std::string function_name = match[1].str();
            //if (!match[2].str().empty()) {
            //    function_name += ":" + match[2].str();
            //}
            functions.push_back(match[2].str());
            start = match.suffix().first;
        }

        //Return extracted functions
        return functions;
    }

    std::vector<std::string> Lua::Service::getScriptFunctions(std::filesystem::path const& path) {
        //Open script file
        std::ifstream script_file(path);
        if (!script_file.is_open()) {
            NIKEE_CORE_WARN("Unable to open Lua script file: {}", path.string());
            return {};
        }

        //Read script into memory
        std::string script_content((std::istreambuf_iterator<char>(script_file)),
            std::istreambuf_iterator<char>());
        script_file.close();

        //Regex pattern for Lua function definitions
        std::regex function_pattern(R"(function\s+([a-zA-Z_][a-zA-Z0-9_]*)(?::([a-zA-Z_][a-zA-Z0-9_]*))?\s*\()");
        std::smatch match;

        //Vector to store extracted functions
        std::vector<std::string> functions;

        //Search for all matches
        auto start = script_content.cbegin();
        auto end = script_content.cend();
        while (std::regex_search(start, end, match, function_pattern)) {
            std::string function_name = match[1].str();
            //if (!match[2].str().empty()) {
            //    function_name += ":" + match[2].str();
            //}
            functions.push_back(match[2].str());
            start = match.suffix().first;
        }

        //Return extracted functions
        return functions;
    }

    std::unordered_set<std::string> Lua::Service::getGlobalLuaFunctions() const {
        return lua_global_funcs;
    }
}
