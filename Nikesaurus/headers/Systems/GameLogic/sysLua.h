/*****************************************************************//**
 * \file   sysLua.h
 * \brief  Lua manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef SYS_LUA_HPP
#define SYS_LUA_HPP

namespace NIKE {
	namespace Lua {

		//Bindable lua interface
		class ILuaBind {
		private:
		public:
			virtual void registerLuaBindings(sol::state& lua_state) = 0;
			virtual ~ILuaBind() = default;
		};
		
		//Lua system
		class System {
		private:

			//Lua State
			std::unique_ptr<sol::state> lua_state;

			//Loaded lua scripts
			std::unordered_map<std::string, std::pair<std::string, sol::table>> scripts;

			//Lua bindable systems
			std::vector<std::shared_ptr<ILuaBind>> systems;
		public:
			System();
			~System() = default;

			//Initialize Lua
			void init();

			//Binding of lua to C++ functions
			void registerBindings();

			//Register lua bindable systems
			void registerLuaSystem(std::shared_ptr<Lua::ILuaBind> system);

			//Load lua script
			std::string loadScript(std::string const& file_path);

			//reload lua script
			void reloadScript(std::string const& script_id);

			//Reload all lua scripts
			void reloadAllScripts();

			//Create entity from prefab
			void shootBullet(int layer_id, const std::string& file_path, const std::string& entity_name);
			//void shootBullet(int layer_id, const std::string& file_path, const std::string& entity_name, const Vector2f& shooter_pos);
			// 
			//Execute lua scripts
			sol::protected_function executeScript(std::string const& script_id, std::string const& function);
		};
	}
}

#endif //!SYS_LUA_HPP
