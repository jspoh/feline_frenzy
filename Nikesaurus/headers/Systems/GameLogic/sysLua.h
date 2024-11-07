/*****************************************************************//**
 * \file   sysLua.h
 * \brief  Lua manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

namespace NIKE {
	namespace Lua {

		//Bindable lua interface
		class LuaBinding {

		};
		
		//Lua system
		class System {
		private:
			//Binding of lua to C++ functions
			void registerBindings();

			//Lua State
			std::unique_ptr<sol::state> lua_state;

			//Loaded lua scripts
			std::unordered_map<std::string, std::pair<std::string, sol::load_result>> scripts;
		public:
			System();
			~System() = default;

			//Initialize Lua
			void init();

			//Load lua script
			void loadScript(std::string const& script_id, std::string const& file_path);

			//Reload all lua scripts
			void reloadScripts();

			//Execute lua scripts
			void executeScript(std::string const& script_id);
		};
	}
}
