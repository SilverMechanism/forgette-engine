module;
#include "defines.h"
#include "luajit/lua.hpp"
export module gm_survival_scriptable;
import core;
import gm_survival;
import game_map;
import forgette;

export
{
	class GM_Survival_SCRIPTABLE
	{
	public:
		static int _start_gm_survival(lua_State* L)
		{
			/* int arg_count = lua_gettop(L);
			if (arg_count != 2)
			{
				lua_pushstring(L, "Error: spawn_entity requires 2 args");
				return 1;
			} */
			
			/* coordinates coords = coordinates(static_cast<float>(lua_tonumber(L, 1)), static_cast<float>(lua_tonumber(L, 2)));
		    lua_pushlightuserdata(L, get_engine()->spawn_entity<DebugUnit>(coords).get()); */
		    
		    get_engine()->active_map->setup_game<GM_Survival>();
		    
		    return 1;
		}
	};
	
	extern "C"
	{
		int start_gm_survival(lua_State* L) 
		{
			return GM_Survival_SCRIPTABLE::_start_gm_survival(L);
		}
	}
}