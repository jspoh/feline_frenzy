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

		//Lua Service
		class NIKE_API Service {
		private:

			//Lua State
			std::unique_ptr<sol::state> lua_state;
		public:
			Service();
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
			sol::table loadScript(std::string const& virtual_path);

			//Get functions from lua script
			std::vector<std::string> getScriptFunctions(std::string const& virtual_path);

			//Execute script based on sol::table object
			sol::object executeScript(sol::table script_table, std::string const& function, int argc, sol::variadic_args const& args);

			//Execute lua script based on asset service id
			sol::object executeScript(std::string const& script_id, std::string const& function, int argc, sol::variadic_args args);
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif //!SYS_LUA_HPP
