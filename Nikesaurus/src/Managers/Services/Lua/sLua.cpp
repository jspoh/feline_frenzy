/*****************************************************************//**
 * \file   sLua.cpp
 * \brief  Lua manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Lua/sLua.h"
#include "Managers/Services/Lua/sLuaBindings.h"

namespace NIKE {

    Lua::Service::Service()
        : lua_state{ std::make_unique<sol::state>() } {
    }

    void Lua::Service::init() {
        //Lua state init
        lua_state->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::io);

       //Register all bindings here
        luaBasicBinds(*lua_state);
        luaKeyBinds(*lua_state);
        luaMathBinds(*lua_state);
    }

    sol::table Lua::Service::loadScript(std::string const& virtual_path) {

        //Resolve virtual path
        auto path = NIKE_PATH_SERVICE->resolvePath(virtual_path);

        //Ensure the extension is ".lua"
        if (path.extension() != ".lua") {
            throw std::runtime_error("Invalid file extension");
        }

        //Load script
        sol::load_result script = lua_state->load_file(path.string());
            
        //Load failure
        if (!script.valid()) {
            sol::error err = script;
            NIKEE_CORE_WARN(err.what());
        }

        //Execute the script to get the table
        sol::protected_function_result result = script();

        //Execute failure
        if (!result.valid()) {
            sol::error err = result;
            NIKEE_CORE_WARN(err.what());
        }

        //Return script table
        sol::table sol_table = result;
        return sol_table;
    }

    std::vector<std::string> Lua::Service::getScriptFunctions(std::string const& virtual_path) {

        //Resolve virtual path
        auto path = NIKE_PATH_SERVICE->resolvePath(virtual_path);

        //Open script file
        std::ifstream script_file(path);
        if (!script_file.is_open()) {
            NIKEE_CORE_ERROR("Unable to open Lua script file: {}", path.string());
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
            if (!match[2].str().empty()) {
                function_name += ":" + match[2].str();
            }
            functions.push_back(function_name);
            start = match.suffix().first;
        }

        //Return extracted functions
        return functions;
    }

    sol::object Lua::Service::executeScript(sol::table script_table, std::string const& function, int argc, sol::variadic_args const& args) {
        //Get the Lua function from the script
        sol::function func = script_table[function];
        if (!func.valid()) {
            throw std::runtime_error("Function not found in script: " + function);
        }

        //Execute the Lua function with arguments
        try {

            //Execute script & return result
            sol::object result = func.call(argc, args);
            return result;
        }
        catch (const sol::error& err) {

            //Log error executing script
            NIKEE_CORE_ERROR(err.what());
            return sol::nil;
        }
    }

    sol::object Lua::Service::executeScript(std::string const& script_id, std::string const& function, int argc, sol::variadic_args args) {
        //Get script table from asset service
        auto script_table = NIKE_ASSETS_SERVICE->getAsset<sol::table>(script_id);
        
        //Get the Lua function from the script
        sol::function func = (*script_table)[function];
        if (!func.valid()) {
            throw std::runtime_error("Function not found in script: " + function);
        }

        //Execute the Lua function with arguments
        try {

            //Execute script & return result
            sol::object result = func.call(argc, args);
            return result;
        }
        catch (const sol::error& err) {

            //Log error executing script
            NIKEE_CORE_ERROR(err.what());
            return sol::nil;
        }
    }
}
