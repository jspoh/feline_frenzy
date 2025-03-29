/*****************************************************************//**
 * \file   sLua.h
 * \brief  Lua manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
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

		//Cached lua script object
		struct ScriptObj {
			std::filesystem::path path;
			std::unordered_map<std::string, LuaValue> configs;
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

			//Convert schema to config
			void convertSchemaToConfig(sol::table const& schema, std::unordered_map<std::string, LuaValue>& configs);

			//Convert config to schema
			void convertConfigToSchema(std::unordered_map<std::string, LuaValue> const& configs, sol::table& schema);

			//Get table keys
			std::vector<std::string> getTableKeys(sol::table const& table);

			//Load script
			ScriptObj loadScript(std::filesystem::path const& path);

			//Init script
			void initScript(std::string const& script_id, std::unordered_map<std::string, LuaValue> const& configs, sol::table& script_instance, bool& init);

			//Get functions from lua script
			std::vector<std::string> getScriptFunctions(std::string const& virtual_path);

			//Get functions from lua script
			std::vector<std::string> getScriptFunctions(std::filesystem::path const& path);

			//Get all lua global functions
			std::unordered_set<std::string> getGlobalLuaFunctions() const;
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif //!SYS_LUA_HPP
