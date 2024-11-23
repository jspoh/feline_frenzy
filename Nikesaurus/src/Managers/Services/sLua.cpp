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
#include "Managers/Services/sLua.h"
#include "Systems/Physics/sysPhysics.h"

namespace NIKE {

    void Lua::Service::registerBindings() {

        //Reigster all lua Service bindings
        for (auto& system : systems) {
            system->registerLuaBindings(*lua_state);
        }

        //Register lua table for key codes
        lua_state->create_named_table("Key",
            "W", NIKE_KEY_W,
            "A", NIKE_KEY_A,
            "S", NIKE_KEY_S,
            "D", NIKE_KEY_D
        );

        //Register lua bindings for key inputs
        lua_state->set_function("isKeyPressed", [](int key)->bool { return NIKE_INPUT_SERVICE->isKeyPressed(key); });
        lua_state->set_function("isKeyTriggered", [](int key)->bool { return NIKE_INPUT_SERVICE->isKeyTriggered(key); });
        lua_state->set_function("iskeyReleased", [](int key)->bool { return NIKE_INPUT_SERVICE->isKeyReleased(key); });

        //Bind Cout for lua
        lua_state->set_function("cout", sol::overload(
            [](const std::string& msg) {
            cout << "[Lua] " << msg << endl;
            },
            [](int value) {
                cout << "[Lua] " << value << endl;
            }
            ));

        //Define the noop function in Lua
        (*lua_state).script(R"(
            function noop()
                -- No operation
            end
        )");
    }

    Lua::Service::Service()
        : lua_state{ std::make_unique<sol::state>() } {
        registerBindings();
    }

    void Lua::Service::init() {
        //Lua state init
        lua_state->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::io);
    }

    void Lua::Service::registerLuaSystem(std::shared_ptr<Lua::ILuaBind> system) {
        system->registerLuaBindings(*lua_state);
        systems.push_back(system);
    }

    std::string Lua::Service::loadScript(std::string const& file_path_str) {

        // Conversion to std::filesys::path
        std::filesystem::path file_path = file_path_str;

        // Check for empty file path
        if (file_path.empty())
            return "";

        // Ensure the extension is ".lua"
        if (file_path.extension() != ".lua") {
            throw std::runtime_error("Invalid file extension");
        }

        //Create script id
        std::string script_id = file_path.stem().string();

        //Load script
        sol::load_result script = lua_state->load_file(file_path.string());
            
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

        //Get script table
        sol::table script_table = result;

        //Store the script table for hot-reloading
        scripts[script_id] = { file_path, std::move(script_table) };
        NIKEE_CORE_INFO("Script: " + script_id + " succesfully loaded from: " + file_path.string());

        //Return script id
        return script_id;
    }

    void Lua::Service::reloadScript(std::string const& script_id) {
        //Check if script is already loaded
        auto it = scripts.find(script_id);
        if (it == scripts.end()) {
            throw std::runtime_error("Script not loaded yet.");
        }

        const std::filesystem::path& file_path = it->second.first;

        //Reload script file
        sol::load_result new_script = lua_state->load_file(file_path.string());

        //Load failure
        if (!new_script.valid()) {
            sol::error err = new_script;
            NIKEE_CORE_WARN(err.what());
        }

        //Execute the script to get the table
        sol::protected_function_result result = new_script();

        //Execute failure
        if (!result.valid()) {
            sol::error err = result;
            NIKEE_CORE_WARN(err.what());
        }

        //Get script table
        sol::table script_table = result;

        //Update new script table
        scripts.at(script_id).second = std::move(script_table);
        NIKEE_CORE_INFO("Script: " + script_id + " succesfully reloaded from: " + file_path.string());
    }

    void Lua::Service::reloadAllScripts() {
        for (auto& script : scripts) {
            //Reload script
            reloadScript(script.first);
        }
    }

    void Lua::Service::loadAllScripts() {
        // Ensure the scripts is fresh each time
        scripts.clear();

        // Iterate through the directory and load valid files
        for (const auto& entry : std::filesystem::directory_iterator(NIKE_ASSETS_SERVICE->getScriptsPath())) {
            if (entry.is_regular_file() && hasValidScriptExtension(entry))
            {
                loadScript(entry.path().string());
            }

        }
    }

    sol::protected_function Lua::Service::executeScript(std::string const& script_id, std::string const& function) {

        //Check if script is already loaded
        auto it = scripts.find(script_id);
        if (it == scripts.end()) {
            throw std::runtime_error("Script not loaded yet.");
        }

        //Access the script table
        sol::table script_table = it->second.second;

        //Return if noop function if no function specified
        if (function == "") {
            return (*lua_state)["noop"];
        }

        //Execute script
        sol::protected_function func = script_table[function];
        if (!func.valid()) {
            NIKEE_CORE_WARN("Invalid function called.");
            return (*lua_state)["noop"];
        }

        //Return function
        return func;
    }

    bool Lua::Service::checkScriptFileExist(const std::string& entry)
    {
        auto it = scripts.find(entry);
        if (it != scripts.end()) {
            return std::filesystem::exists(it->second.first);
        }
        return false;
    }

    std::unordered_map<std::string, std::pair<std::filesystem::path, sol::table>>& Lua::Service::getAllScripts()
    {
        return scripts;
    }

    // Function to extract the function from the Lua script
    std::string Lua::Service::extractFunctionFromScript(const std::string& script_id) {

        // Get path that corresponds to the script id
        std::filesystem::path script_path = scripts[script_id].first;

        std::ifstream script_file(script_path);
        if (!script_file.is_open()) {
            NIKEE_CORE_ERROR("Unable to open Lua script file.");
        }

        std::string script_content((std::istreambuf_iterator<char>(script_file)),
            std::istreambuf_iterator<char>());

        // Regex to match Lua function definitions in tables
        std::regex function_pattern(R"(function\s+([a-zA-Z_][a-zA-Z0-9_]*)(?::([a-zA-Z_][a-zA-Z0-9_]*))?\s*\()");

        std::smatch match;
        if (std::regex_search(script_content, match, function_pattern)) {
            // If a match is found, return the full function name 
            std::string function_name = match[1].str();
            if (match[2].length() > 0) {
                //function_name += ":" + match[2].str();  
                return match[2].str();
            }
            return function_name;
        }

        // If no function is found, return empty string
        return "";
    }

    std::filesystem::path Lua::Service::getScriptPath(const std::string& script_id) const {
        auto it = scripts.find(script_id);
        if (it != scripts.end()) {
            // Return the path associated with the script ID
            return it->second.first; 
        }
        NIKEE_CORE_ERROR("Script ID '{}' not found.", script_id);
        // Return an empty path if not found
        return {}; 
    }
}
