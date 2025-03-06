/*****************************************************************//**
 * \file   sLuaBindings.h
 * \brief  Lua bindings
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef S_LUA_BINDS_HPP
#define S_LUA_BINDS_HPP

namespace NIKE {
	namespace Lua {

		//Lua key binds
		void luaKeyBinds(sol::state& lua_state);

		//Lua basic binds
		void luaBasicBinds(sol::state& lua_state);

		//Lua input binds
		void luaInputBinds(sol::state& lua_state);

		//Lua scenes binds
		void luaSceneBinds(sol::state& lua_state);
	}
}

#endif //!S_LUA_BINDS_HPP