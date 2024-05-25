module;
#include "luajit/lua.hpp"
export module lua_manager;
import core;
import std;

int lua_print(lua_State* L) {
    int nargs = lua_gettop(L);
    for (int i = 1; i <= nargs; ++i) {
        if (lua_isstring(L, i)) {
            std::cout << lua_tostring(L, i);
        } else {
            std::cout << lua_typename(L, lua_type(L, i));
        }
        if (i < nargs) {
            std::cout << "\t";
        }
    }
    std::cout << std::endl;
    return 0;
}

export
{
	class LuaManager
	{
	public:
		std::vector<std::function<void(coordinates<float>)>> spawn_functions = {
			
		};
		
		LuaManager();
		~LuaManager();
		
		lua_State* lua_state = nullptr;
		
		bool run_lua_script(const std::wstring &path);
	};
}

LuaManager::LuaManager()
{
	lua_state = luaL_newstate();
	
	if (lua_state != nullptr)
	{
		luaL_openlibs(lua_state);
		lua_pushcfunction(lua_state, lua_print);
		lua_setglobal(lua_state, "print");
	}
}

LuaManager::~LuaManager()
{
	if (lua_state != nullptr)
	{
		lua_close(lua_state);
	}
}

bool LuaManager::run_lua_script(const std::wstring &path)
{
	std::string script_path = wstring_to_string(path);
	
	if (luaL_loadfile(lua_state, script_path.c_str()) || lua_pcall(lua_state, 0, 0, 0)) {
        std::cerr << "Error: " << lua_tostring(lua_state, -1) << std::endl;
        return false;
    }
    return true;
}