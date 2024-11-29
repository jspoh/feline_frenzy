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
#include "Managers/Services/sLua.h"
#include "Systems/Physics/sysPhysics.h"

namespace NIKE {

    // Bullet functions

    void Lua::System::shootBullet(int layer_id, const std::string& file_path, const std::string& entity_name, const Vector2f& shooter_pos, const float& offset) {
        //NIKEE_CORE_INFO("SHOOT BULLET CALLED");
        if (layer_id < static_cast<int>(NIKE_SCENES_SERVICE->getLayerCount())) {
            // Create entity function call ( Defaulted to the base layer for now )
            Entity::Type new_id = NIKE_ECS_MANAGER->createEntity(layer_id);
            //NIKEE_CORE_INFO("Bullet entity created with ID: " + std::to_string(new_id));

            // If empty string, assign default string
            std::string name = entity_name.empty() ? "entity_" + std::to_string(new_id) : entity_name;

            // Save entity_name string
            //NIKE_IMGUI_SERVICE->addEntityRef(name, new_id);

            // Load entity with prefab
            //std::string prefab_full_path = NIKE_ASSETS_SERVICE->getPrefabsPath() + file_path;
            std::string prefab_full_path = "assets/Prefabs/" + file_path;
            NIKE_SERIALIZE_SERVICE->loadEntityFromFile(new_id, prefab_full_path);

            // Get mouse position
            Vector2f mouse_pos = {
                NIKE_INPUT_SERVICE->getMousePos().x - (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x / 2),
                -(NIKE_INPUT_SERVICE->getMousePos().y - (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y / 2))
            };

            // Calculate direction
            Vector2f direction = mouse_pos - shooter_pos;
            direction.normalize(); 

            // Offset spawn position of bullet
            Vector2f bullet_pos = shooter_pos + (direction * offset);

            // Set bullet's position
            auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(new_id);
            if (e_transform_comp.has_value()) {
                e_transform_comp.value().get().position = bullet_pos;
            }

            // Apply physics
            auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(new_id);
            if (e_physics_comp.has_value()) {
                // Set force
                e_physics_comp.value().get().force = { direction.x * 1000.f, direction.y * 1000.f };
            }
        }
    }

    void Lua::System::enemyBullet(int layer_id, const std::string& file_path, const std::string& entity_name, const Vector2f& enemy_pos, const Vector2f& player_pos, const float& offset) {
        if (layer_id < static_cast<int>(NIKE_SCENES_SERVICE->getCurrScene()->getLayerCount())) {
            // Create entity function call
            Entity::Type new_id = NIKE_ECS_MANAGER->createEntity(layer_id);

            // If empty string, assign default string
            std::string name = entity_name.empty() ? "entity_" + std::to_string(new_id) : entity_name;

            // Save entity_name string
            NIKE_IMGUI_SERVICE->addEntityRef(name, new_id);

            // Load entity with prefab
            std::string prefab_full_path = NIKE_ASSETS_SERVICE->getPrefabsPath() + file_path;
            NIKE_SERIALIZE_SERVICE->loadEntityFromFile(new_id, prefab_full_path);

            // Calculate direction
            Vector2f direction = player_pos - enemy_pos;
            direction.normalize();

            // Offset spawn position of bullet
            Vector2f bullet_pos = enemy_pos + (direction * offset);

            // Set bullet's position
            auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(new_id);
            if (e_transform_comp.has_value()) {
                e_transform_comp.value().get().position = bullet_pos;
            }

            // Apply physics
            auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(new_id);
            if (e_physics_comp.has_value()) {
                // Set force
                e_physics_comp.value().get().force = { direction.x * 1.f, direction.y * 1.f };
            }
        }
    }

    void Lua::System::registerBindings() {

        //Reigster all lua Service bindings
        for (auto& system : systems) {
            system->registerLuaBindings(*lua_state);
        }

        //Register lua table for key codes
        lua_state->create_named_table("Key",
            // Movement
            "W", NIKE_KEY_W,
            "A", NIKE_KEY_A,
            "S", NIKE_KEY_S,
            "D", NIKE_KEY_D,
            // Numbers (For cheat code currently)
            "0", NIKE_KEY_0,
            "1", NIKE_KEY_1,
            "2", NIKE_KEY_2,
            "3", NIKE_KEY_3,
            "4", NIKE_KEY_4,
            "8", NIKE_KEY_8, 
            "9", NIKE_KEY_9  
        );

        //Register lua bindings for key inputs
        lua_state->set_function("isKeyPressed", [](int key)->bool { return NIKE_INPUT_SERVICE->isKeyPressed(key); });
        lua_state->set_function("isKeyTriggered", [](int key)->bool { return NIKE_INPUT_SERVICE->isKeyTriggered(key); });
        lua_state->set_function("isKeyReleased", [](int key)->bool { return NIKE_INPUT_SERVICE->isKeyReleased(key); });

        //Register lua binding for player bullet
        lua_state->set_function("shootBullet", [this](int layer_id, const std::string& file_path, const std::string& entity_name, float shooter_x, float shooter_y, float offset) {
            Vector2f shooter_pos{ shooter_x, shooter_y };
            this->shootBullet(layer_id, file_path, entity_name, shooter_pos, offset);
            });

        //Register lua binding for enemy bullet
        lua_state->set_function("enemyBullet", [this](int layer_id, const std::string& file_path, const std::string& entity_name, float enemy_x, float enemy_y, float player_x, float player_y, float offset) {
            Vector2f enemy_pos{ enemy_x, enemy_y }, player_pos{ player_x, player_y };
            this->enemyBullet(layer_id, file_path, entity_name, enemy_pos, player_pos, offset);
            });

        // Register lua binding for cheat mode functions
        
        // Mouse position getter
        lua_state->set_function("getMousePos", [this]() -> sol::table {
            // Get mouse position
            Vector2f mouse_pos = {
                NIKE_INPUT_SERVICE->getMousePos().x - (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x / 2),
                -(NIKE_INPUT_SERVICE->getMousePos().y - (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y / 2))
            };
            sol::table pos_table = lua_state->create_table();
            pos_table["x"] = mouse_pos.x;
            pos_table["y"] = mouse_pos.y;
            return pos_table;
            });

        // Entity position setter
        lua_state->set_function("setPosition", [](Entity::Type entity_id, float x, float y) {
            auto transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity_id);
            if (transform.has_value()) {
                transform.value().get().position = { x, y };
            }
            });

        // God mode toggle
        lua_state->set_function("setGodMode", [](Entity::Type entity_id, bool enable) {
            auto health_comp = NIKE_ECS_MANAGER->getEntityComponent<Health::Health>(entity_id);
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
        lua_state->set_function("setHighDamage", [](Entity::Type entity_id, bool enable) {
            auto damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Damage::Damage>(entity_id);
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

        //// Iterate through the directory and load valid files
        //for (const auto& entry : std::filesystem::directory_iterator(NIKE_ASSETS_SERVICE->getScriptsPath())) {
        //    if (entry.is_regular_file() && hasValidScriptExtension(entry))
        //    {
        //        loadScript(entry.path().string());
        //    }

        //}
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
