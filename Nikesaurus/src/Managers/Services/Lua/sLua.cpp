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

    nlohmann::json Lua::Script::serialize() const {
        return	{
        {"Script_ID", script_id},
        {"Start_Function", start_function},
        {"Update_Function", update_function},
        {"Named_Args", named_args}
        };
    }

    void Lua::Script::deserialize(nlohmann::json const& data) {
        script_id = data.value("Script_ID", "");
        start_function = data.value("Start_Function", "");
        update_function = data.value("Update_Function", "");

        if (data.contains("Named_Args")) {
            for (const auto& [key, value] : data.at("Named_Args").items()) {
                named_args[key] = value.get<LuaValue>();
            }
        }
    }

    nlohmann::json Lua::Script::overrideSerialize(Script const& other_script) const {
        nlohmann::json delta;

        if (script_id != other_script.script_id) {
            delta["Script_ID"] = script_id;
        }
        if (start_function != other_script.start_function) {
            delta["Start_Function"] = start_function;
        }
        if (update_function != other_script.update_function) {
            delta["Update_Function"] = update_function;
        }
        if (named_args != other_script.named_args) {
            delta["Named_Args"] = named_args;
        }

        return delta;
    }

    void Lua::Script::overrideDeserialize(nlohmann::json const& data) {
        if (data.contains("Script_ID")) {
            script_id = data["Script_ID"];
        }

        if (data.contains("Start_Function")) {
            start_function = data["Start_Function"];
        }

        if (data.contains("Update_Function")) {
            update_function = data["Update_Function"];
        }

        if (data.contains("Named_Args")) {
            for (const auto& [key, value] : data.at("Named_Args").items()) {
                named_args[key] = value.get<LuaValue>();
            }
        }
    }

    std::shared_ptr<sol::load_result> Lua::Service::getLuaAssset(std::string const& script_id) const {
        //Get script table from asset service
       return NIKE_ASSETS_SERVICE->getAsset<sol::load_result>(script_id);
    }

    void Lua::Service::uncacheLuaAsset(std::string const& script_id) const {
        NIKE_ASSETS_SERVICE->uncacheAsset(script_id);
    }

    sol::table Lua::Service::convertScriptArgs(Script const& script) const {
        //Convert arguments to a Lua table
        sol::table lua_args = lua_state->create_table();
        for (const auto& pair : script.named_args) {
            const auto& key = pair.first;
            const auto& value = pair.second;

            std::visit([&lua_args, &key](auto&& arg) {
                lua_args[key] = arg;
                }, value);
        }

        return lua_args;
    }

    void Lua::Service::init() {
        lua_state = std::make_unique<sol::state>();

        //Lua state init
        lua_state->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::io);

        //Register all bindings here
        luaBasicBinds(*lua_state);
        luaKeyBinds(*lua_state);
        luaInputBinds(*lua_state);
        luaSceneBinds(*lua_state);
        luaECSBinds(*lua_state);
        luaGameBinds(*lua_state);

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

    sol::load_result Lua::Service::loadScript(std::string const& virtual_path) {

        //Resolve virtual path
        auto path = NIKE_PATH_SERVICE->resolvePath(virtual_path);

        //Lua error
        if (lua_state == nullptr) {
            throw std::runtime_error("Lua Error!");
        }

        // Check file size (ensure not empty)
        if (std::filesystem::file_size(path) == 0) {
            throw std::runtime_error("File is empty");
        }

        //Ensure the extension is ".lua"
        if (path.extension() != ".lua") {
            throw std::runtime_error("Invalid file extension");
        }

        //Load script
        sol::load_result script = lua_state->load_file(path.string());
            
        //Load failure
        if (!script.valid()) {
            sol::error err = script;
            throw std::runtime_error(err.what());
        }

        return script;
    }

    sol::load_result Lua::Service::loadScript(std::filesystem::path const& path) {
        //Ensure the extension is ".lua" &  path is value
        if (path.extension() != ".lua" || !std::filesystem::exists(path.string())) {
            throw std::runtime_error("Invalid file extension or invalid path.");
        }

        //Lua error
        if (lua_state == nullptr) {
            throw std::runtime_error("Lua Error!");
        }

        // Check file size (ensure not empty)
        if (std::filesystem::file_size(path) == 0) {
            throw std::runtime_error("File is empty");
        }

        //Load script
        sol::load_result script = lua_state->load_file(path.string());

        //Load failure
        if (!script.valid()) {
            sol::error err = script;
            throw std::runtime_error(err.what());
        }

        return script;
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
