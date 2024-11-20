/*****************************************************************//**
 * \file   sysLua.cpp
 * \brief  Lua manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/GameLogic/sysLua.h"
#include "Systems/Physics/sysPhysics.h"

namespace NIKE {
    void Lua::System::shootBullet(int layer_id, const std::string& file_path, const std::string& entity_name = "", const Vector2f& shooter_pos = { 0.f, 0.f }) {
    //void Lua::System::shootBullet(int layer_id, const std::string& file_path, const std::string& entity_name = "") {
        NIKEE_CORE_INFO("SHOOT BULLET CALLED");
        if (layer_id < static_cast<int>(NIKE_SCENES_SERVICE->getCurrScene()->getLayerCount())) {
            // Create entity function call ( Defaulted to the base layer for now )
            Entity::Type new_id = NIKE_ECS_MANAGER->createEntity(layer_id);
            NIKEE_CORE_INFO("Bullet entity created with ID: " + std::to_string(new_id));

            // If empty string, assign default string
            std::string name = entity_name.empty() ? "entity_" + std::to_string(new_id) : entity_name;

            // Save entity_name string
            NIKE_IMGUI_SERVICE->addEntityRef(name, new_id);

            // Load entity with prefab
            std::string prefab_full_path = NIKE_ASSETS_SERVICE->getPrefabsPath() + file_path;
            NIKE_SERIALIZE_SERVICE->loadEntityFromFile(new_id, prefab_full_path);

            Vector2f bullet_pos = shooter_pos;
            // Transform
            auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(new_id);
            if (e_transform_comp.has_value()) {
                //Set entity position to (0, 0) for now
                // Will change this to the coord of whatever is spawning the boolet
                e_transform_comp.value().get().position = bullet_pos;
            }

            // Physics
            auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(new_id);
            if (e_physics_comp.has_value()) {
                // Get mouse position
                Vector2f mouse_pos = {
                    NIKE_INPUT_SERVICE->getMousePos().x - (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x / 2),
                    -(NIKE_INPUT_SERVICE->getMousePos().y - (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y / 2))
                };

                // Calculate direction vector
                Vector2f direction = mouse_pos - bullet_pos;

                // Normalizing direction vector
                direction.normalize();

                e_physics_comp.value().get().velocity = { direction.x * 1000.f, direction.y * 1000.f };
            }
        }
    }

    void Lua::System::registerBindings() {

        //Reigster all lua system bindings
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

        //Register lua binding for prefab loading
        lua_state->set_function("shootBullet", [this](int layer_id, const std::string& file_path, const std::string& entity_name, float shooter_x, float shooter_y) {
            Vector2f shooter_pos{ shooter_x, shooter_y };
            this->shootBullet(layer_id, file_path, entity_name, shooter_pos);
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

    Lua::System::System()
        : lua_state{ std::make_unique<sol::state>() } {
        registerBindings();
    }

    void Lua::System::init() {
        //Lua state init
        lua_state->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::io);
    }

    void Lua::System::registerLuaSystem(std::shared_ptr<Lua::ILuaBind> system) {
        system->registerLuaBindings(*lua_state);
        systems.push_back(system);
    }

    std::string Lua::System::loadScript(std::string const& file_path) {

        //Check for empty file path
        if (file_path == "")
            return "";

        //Create script id
        size_t lua_pos = file_path.find_first_of(".lua");

        //Throw error if file extension is wrong
        if (lua_pos == std::string::npos) {
            throw std::runtime_error("Invalid file extension");
        }

        //Create script id
        std::string script_id = file_path.substr(file_path.find_last_of('/') + 1, file_path.find_last_of('.') - file_path.find_last_of('/') - 1);

        //Load script
        sol::load_result script = lua_state->load_file(file_path);

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
        scripts[script_id].second = std::move(script_table);
        NIKEE_CORE_INFO("Script: " + script_id + " succesfully loaded from: " + scripts.at(script_id).first);

        //Return script id
        return script_id;
    }

    void Lua::System::reloadScript(std::string const& script_id) {
        //Check if script is already loaded
        auto it = scripts.find(script_id);
        if (it == scripts.end()) {
            throw std::runtime_error("Script not loaded yet.");
        }

        //Reload script file
        sol::load_result new_script = lua_state->load_file(scripts.at(script_id).first);

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
        NIKEE_CORE_INFO("Script: " + script_id + " succesfully reloaded from: " + scripts.at(script_id).first);
    }

    void Lua::System::reloadAllScripts() {
        for (auto& script : scripts) {

            //Reload script
            reloadScript(script.first);
        }
    }

    sol::protected_function Lua::System::executeScript(std::string const& script_id, std::string const& function) {

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
}
