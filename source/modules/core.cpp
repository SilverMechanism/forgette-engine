module;
#include <cassert>
#include <cwchar>
#include "defines.h"
#ifdef WIN64
    #include <windows.h>
#endif

module core;
import std;
import :core_math;
import :coordinates;
import :smart_pointers;
import :entity_class;

static std::wstring exe_path;

std::wstring get_exe_path()
{
    return exe_path;
}

std::wstring get_exe_dir()
{
    std::wstring exe_dir = exe_path;

    int i = 0;
    for (i = static_cast<int>(exe_dir.size()); i > -1; i--)
    {
        if (exe_dir[i] == '\\')
        {
            break;
        }
    }

    i = static_cast<int>(exe_dir.size()) - i;

    for (i; i > 0; i--)
    {
        exe_dir.pop_back();
    }

    return exe_dir + L"\\";
}

void set_exe_path(std::wstring new_path)
{
    exe_path = new_path;
}

#ifdef WIN64
std::string wstring_to_string(const std::wstring& to_convert)
{
    if (to_convert.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &to_convert[0], (int)to_convert.size(), NULL, 0, NULL, NULL);
    std::string conversion(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &to_convert[0], (int)to_convert.size(), &conversion[0], size_needed, NULL, NULL);
    return conversion;
}

std::wstring string_to_wstring(const std::string& to_convert)
{
    std::mbstate_t state = std::mbstate_t();
    const char* src = to_convert.c_str();
    std::size_t len;
    mbsrtowcs_s(&len, nullptr, 0, &src, 0, &state);
    std::vector<wchar_t> wstr(len);
    mbsrtowcs_s(&len, wstr.data(), len, &src, len, &state);
    return std::wstring(wstr.data());
}
#endif
