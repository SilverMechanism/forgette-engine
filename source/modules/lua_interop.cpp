module;
#include "luajit/lua.hpp"
export module lua_interop;
import debug_unit_scriptable;

export namespace LuaInterop
{
	void register_functions(lua_State* lua_state);
}

void LuaInterop::register_functions(lua_State* lua_state)
{
	lua_register(lua_state, "spawn_debug_unit", spawn_debug_unit);
}