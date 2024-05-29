module;
#include "defines.h"
#include "luajit/lua.hpp"
export module debug_unit_scriptable;
import core;
import forgette;
import debug_unit;

export
{
	class DebugUnit_SCRIPTABLE
	{
	public:
		static int _spawn_debug_unit(lua_State* L)
		{
			int arg_count = lua_gettop(L);
			if (arg_count != 2)
			{
				lua_pushstring(L, "Error: spawn_entity requires 2 args");
				return 1;
			}
			
			coordinates coords = coordinates(static_cast<float>(lua_tonumber(L, 1)), static_cast<float>(lua_tonumber(L, 2)));
		    lua_pushlightuserdata(L, get_engine()->spawn_entity<DebugUnit>(coords).get());
		    
		    return 1;
		}
	};
	
	extern "C"
	{
		int spawn_debug_unit(lua_State* L) 
		{
			return DebugUnit_SCRIPTABLE::_spawn_debug_unit(L);
		}
	}
}