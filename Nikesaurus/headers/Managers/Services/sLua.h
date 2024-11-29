/*****************************************************************//**
 * \file   sLua.h
 * \brief  Lua manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef SYS_LUA_HPP
#define SYS_LUA_HPP

namespace NIKE {
	//Temporary Disable DLL Export Warning
	#pragma warning(disable: 4251)
	namespace Lua {
		//Bindable lua interface
		class ILuaBind {
		private:
		public:
			virtual void registerLuaBindings(sol::state& lua_state) = 0;
			virtual ~ILuaBind() = default;
		};

		//Lua Service
		class NIKE_API Service {
		private:

			//Lua State
			std::unique_ptr<sol::state> lua_state;

			//Loaded lua scripts
			std::unordered_map<std::string, std::pair<std::filesystem::path, sol::table>> scripts;

			//Lua bindable systems
			std::vector<std::shared_ptr<ILuaBind>> systems;
		public:
			Service();
			~Service() = default;

			//Initialize Lua
			void init();

			//Binding of lua to C++ functions
			void registerBindings();

			//Register lua bindable systems
			void registerLuaSystem(std::shared_ptr<Lua::ILuaBind> system);

			//Load lua script
			std::string loadScript(std::string const& file_path_str);

			//reload lua script
			void reloadScript(std::string const& script_id);

			//Reload all lua scripts
			void reloadAllScripts();

			//For shooting component
			void shootBullet(int layer_id, const std::string& file_path, const std::string& entity_name, const Vector2f& shooter_pos, const float& offset);

			//For enemy component
			void enemyBullet(int layer_id, const std::string& file_path, const std::string& entity_name, const Vector2f& enemy_pos, const Vector2f& player_pos, const float& offset);
			
			//Reload all lua scripts
			void loadAllScripts();

			// Check if lua script exist
			bool checkScriptFileExist(const std::string& entry);

			// Gettor for container
			std::unordered_map<std::string, std::pair<std::filesystem::path, sol::table>>& getAllScripts();

			//Execute lua scripts
			sol::protected_function executeScript(std::string const& script_id, std::string const& function);

			// For UI runtime loading
			std::string extractFunctionFromScript(const std::string& script_id);

			std::filesystem::path getScriptPath(const std::string& script_id) const;
		};
	}
	//Re-enable DLL Export warning
	#pragma warning(default: 4251)
}

#endif //!SYS_LUA_HPP
