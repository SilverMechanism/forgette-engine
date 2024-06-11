module;
#include "luajit/lua.hpp"
export module lua_interop;

export namespace LuaInterop
{
	void register_functions(lua_State* lua_state);
}

void LuaInterop::register_functions(lua_State* lua_state)
{
}