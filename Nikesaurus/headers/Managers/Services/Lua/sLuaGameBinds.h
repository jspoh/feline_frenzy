/*****************************************************************//**
 * \file   sLuaGameBindings.h
 * \brief  Lua Game bindings
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (100%)
 * \date   March 2025
 * All content 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/


#ifndef S_LUA_GAME_BINDS_HPP
#define S_LUA_GAME_BINDS_HPP

namespace NIKE {
	namespace Lua {
		//Lua game binds
		void luaGameBinds(sol::state& lua_state);

		// Player cooldown
		extern const float player_shot_cooldown;
		extern float player_last_shot_time;

	}
}

#endif //!S_LUA_GAME_BINDS_HPP