/*****************************************************************//**
 * \file   sLua.h
 * \brief  Lua manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef S_LUA_HPP
#define S_LUA_HPP

namespace NIKE {
	namespace Lua {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		//Define a Lua-compatible value type
		using LuaValue = std::variant<int, float, std::string, bool>;

		//Lua script component
		struct NIKE_API Script {
			std::string script_id;
			std::string function;
			std::unordered_map<std::string, LuaValue> named_args;

			Script() : script_id{ "" }, function{ "" } {}

			//Serialize script comp
			nlohmann::json serialize() const;

			//Deserialize script comp
			void deserialize(nlohmann::json const& data);

			//Override Serialization
			nlohmann::json overrideSerialize(Script const& other_script) const;

			//Override Deserialize
			void overrideDeserialize(nlohmann::json const& data);
		};

		//Lua Service
		class NIKE_API Service {
		private:

			//Lua State
			std::unique_ptr<sol::state> lua_state;

			//List of global functions to use in lua
			std::unordered_set<std::string> lua_global_funcs;

			//Internal get sol table asset
			std::shared_ptr<sol::load_result> getLuaAssset(std::string const& script_id) const;

			//Uncache lua asset
			void uncacheLuaAsset(std::string const& script_id) const;

			//Convert script to sol::table args
			sol::table convertScriptArgs(Script const& script) const;
		public:
			Service() = default;
			~Service() = default;

			//Initialize Lua
			void init();

			//Register lua bind to function
			template <typename Func>
			void bindFunction(std::string const& name, Func&& func) {
				lua_state->set_function(name, std::forward<Func>(func));
			}

			//Register lua bind to function table
			template <typename Func>
			void bindFunctionToTable(std::string const& table_name, std::string const& func_name, Func&& func) {
				sol::table table = lua_state->get_or<sol::table>(table_name);
				table.set_function(func_name, std::forward<Func>(func));
			}

			//Register lau bind to new type
			template<typename Type>
			void bindType(sol::variadic_args args) {
				lua_state->new_usertype<Type>(args);
			}

			//Load lua script
			sol::load_result loadScript(std::string const& virtual_path);

			//Load lua script
			sol::load_result loadScript(std::filesystem::path const& path);

			//Get functions from lua script
			std::vector<std::string> getScriptFunctions(std::string const& virtual_path);

			//Get functions from lua script
			std::vector<std::string> getScriptFunctions(std::filesystem::path const& path);

			//Execute script based on sol::table object
			template<typename ...Args>
			sol::object executeScript(sol::load_result& script, std::string const& function, int argc, Args&&... args) {

				try {
					//Check if script is loaded correctly
					if (!script.valid()) {
						throw std::runtime_error("Invalid Lua script: script is not loaded correctly.");
					}

					//Execute script
					sol::protected_function_result result = script();

					//No function to be called
					if (function.empty()) {
						//Execute script table
						if (result.get_type() == sol::type::table) {
							return sol::make_object(script.lua_state(), result);
						}

						//Execute script function
						else {
							sol::object returned_value = result[0];
							sol::function lua_function = returned_value.as<sol::function>();
							return lua_function();
						}
					}

					//Check if result here is not a table
					if (result.get_type() != sol::type::table) {
						throw std::runtime_error("Error! Cant call a function from a function script!");
					}

					//Convert result into a table
					sol::table script_table = result;

					//Function to be called
					sol::function script_function = script_table[function];
					if (!script_function.valid()) {
						throw std::runtime_error("Function not found in script table: " + function);
					}

					//Execute the Lua function with provided arguments
					return script_function(argc, std::forward<Args>(args)...);
				}
				catch (std::runtime_error const& e) {
					NIKEE_CORE_WARN(e.what());
					return sol::nil;
				}
			}

			//Execute lua script based on asset service id
			template<typename ...Args>
			sol::object executeScript(Script const& script) {

				try {
					//Get script table from asset service
					auto loaded_script = getLuaAssset(script.script_id);

					//Check if script is loaded correctly
					if (!loaded_script->valid()) {
						throw std::runtime_error("Invalid Lua script: script is not loaded correctly.");
					}

					//Execute script
					sol::protected_function_result result = (*loaded_script)();

					//No function to be called
					if (script.function.empty()) {

						//Execute script table
						if (result.get_type() == sol::type::table) {
							return sol::make_object(loaded_script->lua_state(), result);
						}

						//Execute script function
						else {
							sol::object returned_value = result[0];
							sol::function lua_function = returned_value.as<sol::function>();
							return lua_function();
						}
					}

					//Check if result here is not a table
					if (result.get_type() != sol::type::table) {
						throw std::runtime_error("Error! Cant call a function from a function script!");
					}

					//Convert result into a table
					sol::table script_table = result;

					//Function to be called
					sol::function script_function = script_table[script.function];
					if (!script_function.valid()) {
						throw std::runtime_error("Function not found in script table: " + script.function);
					}

					//Execute the Lua function with provided arguments
					return script_function(script.named_args.size(), convertScriptArgs(script));
				}
				catch (std::runtime_error const& e) {
					NIKEE_CORE_WARN(e.what());
					uncacheLuaAsset(script.script_id);
					return sol::nil;
				}
			}

			//Get all lua global functions
			std::unordered_set<std::string> getGlobalLuaFunctions() const;
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif //!SYS_LUA_HPP
