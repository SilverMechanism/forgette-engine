local exports = {}

-- Specify the source directory
local source_directory = "..\\source\\modules"

-- Specify the destination directory for the new files
local destination_directory = "..\\source\\scriptables"

local function snake_to_camel(snake_string)
	local substrings = {}
	for substring in snake_string:gmatch("[^_]+") do
		table.insert(substrings, substring)
	end
	for i, substring in ipairs(substrings) do
		substrings[i] = substring:gsub("^%l", string.upper)
	end
	return table.concat(substrings)
end

local function generate_scriptable_unit(module_name)
	local base_class = snake_to_camel(module_name)
	local spawn_function = 'spawn_' .. module_name
	local full_function_name = base_class .. '_SCRIPTABLE::_' .. spawn_function
	local template = --
[[module;
#include "defines.h"
#include "luajit/lua.hpp"
export module ]] .. module_name .. [[_scriptable;
import core;
import forgette;
import ]] .. module_name .. [[;

export
{
	class ]] .. base_class .. [[_SCRIPTABLE
	{
	public:
		static int _]] .. spawn_function .. [[(lua_State* L)
		{
			int arg_count = lua_gettop(L);
			if (arg_count != 2)
			{
				lua_pushstring(L, "Error: spawn_entity requires 2 args");
				return 1;
			}
			
			coordinates coords = coordinates(static_cast<float>(lua_tonumber(L, 1)), static_cast<float>(lua_tonumber(L, 2)));
		    lua_pushlightuserdata(L, get_engine()->spawn_entity<]] .. base_class .. [[>(coords).get());
		    
		    return 1;
		}
	};
	
	extern "C"
	{
		int ]] .. spawn_function .. [[(lua_State* L) 
		{
			return ]] .. full_function_name .. [[(L);
		}
	}
}]]
	local module_table = {}
	module_table[module_name] = spawn_function
	table.insert(exports, module_table)
	return template
end

local function generate_lua_interop()
	local template_0 = --
[[module;
#include "luajit/lua.hpp"
export module lua_interop;
]]
	local template_1 = --
[[

export namespace LuaInterop
{
	void register_functions(lua_State* lua_state);
}

void LuaInterop::register_functions(lua_State* lua_state)
{]]
	local template_2 = --
[[

}]]
	for i, modules in ipairs(exports) do
		for module_name, function_name in pairs(modules) do
			template_1 = 'import ' .. module_name .. '_scriptable;\n' .. template_1 .. '\n\tlua_register(lua_state, "' .. function_name .. '", ' .. function_name .. ');'
		end
	end
	return template_0 .. template_1 .. template_2
end

-- Function to process a single file
local function process_file(file_path)
    local file = io.open(file_path, "r")
    if file then
        local content = file:read("*all")
        file:close()

        -- Check if the file contains the target line
        local script_set = content:match("SCRIPTABLE_UNIT%((%a*)%)")
        if script_set then
            -- Extract the base file name and extension
            local base_name = file_path:match("([^\\]+)%.cpp$")
            if base_name then
                -- Create the new file name with .scriptable. inserted
                local new_file_name = base_name .. ".scriptable.cpp"
                local new_file_path = destination_directory .. "\\" .. new_file_name

                -- Create the new empty file
                local new_file = io.open(new_file_path, "w")
                if new_file then
                    new_file:write(generate_scriptable_unit(base_name))
                    new_file:close()
                else
                    print("Failed to create new file: " .. new_file_path)
                end
            else
                print("Invalid file name: " .. file_path)
            end
        end
    else
        print("Failed to open file for reading: " .. file_path)
    end
end

-- Function to scan the source directory and its subdirectories
local function scan_directory(dir_path)
    for file in io.popen("dir \"" .. dir_path .. "\" /b /s"):lines() do
        if file:match("%.cpp$") then
            process_file(file)
        end
    end
end

-- Create the destination directory if it doesn't exist
os.execute('powershell -command "New-Item -ItemType Directory -Path ' .. destination_directory .. '\" > $null')

-- Start scanning the source directory
scan_directory(source_directory)

print("\nExports:")
print("==================================")
for _, modules in ipairs(exports) do
    for module_name, function_name in pairs(modules) do
    	print()
        print(module_name .. ':\n\t' .. function_name)
        print()
    end
end
print("==================================")
print("End of exports")
print()

local interop_file = io.open(source_directory .. "\\lua_interop.cpp", "w")
if interop_file then
	interop_file:write(generate_lua_interop())
end