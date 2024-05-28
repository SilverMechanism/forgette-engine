module;
#include "sqlitecpp/SQLiteCpp.h"
#include <iostream>
#include <windows.h>
export module database;

// this sucks but what can you do
std::wstring get_exe_dir()
{
    wchar_t buffer[MAX_PATH];
    DWORD result = GetModuleFileNameW(NULL, buffer, MAX_PATH);

    std::wstring exe_path(buffer);
	
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

std::string wstring_to_string(const std::wstring &to_convert)
{
	if (to_convert.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &to_convert[0], (int)to_convert.size(), NULL, 0, NULL, NULL);
    std::string conversion(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &to_convert[0], (int)to_convert.size(), &conversion[0], size_needed, NULL, NULL);
    return conversion;
}
// ...

export namespace database
{
	void print_db(std::wstring path);
	
	// paths are relative to the executable directory
	struct TileDefinition
	{
		std::string name;
		std::string path;
		std::uint8_t atlas_position_x;
		std::uint8_t atlas_position_y;
		std::uint8_t scale = 1;
	};
	
	void tiles_add_entry(TileDefinition entry);
	TileDefinition tiles_get_entry(std::string name);
	
	std::string get_sprite_path(std::string sprite_name);
}

void database::print_db(std::wstring path)
{
	SQLite::Database db(path, SQLite::OPEN_READWRITE);
	
	SQLite::Statement query(db, "SELECT name FROM sqlite_master WHERE type='table';");
	while (query.executeStep())
	{
		std::string table_name = query.getColumn(0);
		std::string select_all_query = "SELECT * FROM " + table_name;
		SQLite::Statement select_all(db, select_all_query);
		
		int column_count = select_all.getColumnCount();
		for (int column = 0; column < column_count; column++)
		{
			std::cout << select_all.getColumnName(column) << "\t";
		}
		std::cout << std::endl;
		
		while (select_all.executeStep())
		{
			for (int column = 0; column < column_count; column++)
			{
				std::cout << select_all.getColumn(column) << "\t";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

namespace database
{
	void tiles_add_entry(TileDefinition entry)
	{
		/* SQLite::Database db("database\\tiles.db", SQLite::OPEN_READWRITE);
		SQLite::Statement query(db, "INSERT INTO tiles (name, path, index, scale) VALUES (?, ?, ?, ?)");
		query.bind(1, entry.name);
		query.bind(2, entry.path);
		query.bind(3, static_cast<int>(entry.index));
		query.bind(4, static_cast<int>(entry.scale));
		
		query.exec(); */
	}
	
	std::string make_db_path(std::string db_name)
	{
		return wstring_to_string(get_exe_dir()) + "database\\" + db_name + ".db";
	}
	
	TileDefinition tiles_get_entry(std::string name)
	{
		TileDefinition entry;
		// std::cout << "SQLite name search: " << name << std::endl;

		try
		{
			std::string db_path = wstring_to_string(get_exe_dir()) + "database\\tiles.db";
			SQLite::Database db(db_path, SQLite::OPEN_READWRITE);
			SQLite::Statement query(db, "SELECT name, path, atlas_x, atlas_y, scale FROM tiles WHERE name = ?");
			query.bind(1, name);

			if (query.executeStep())
			{
				entry.name = query.getColumn(0).getString();
				entry.path = query.getColumn(1).getString();
				entry.atlas_position_x = static_cast<std::uint8_t>(query.getColumn(2).getInt());
				entry.atlas_position_y = static_cast<std::uint8_t>(query.getColumn(3).getInt());
				entry.scale = static_cast<std::uint8_t>(query.getColumn(4).getInt());
			}
			else
			{
				assert(true);
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << "SQLite exception: " << e.what() << std::endl;
			assert(true);
		}

		return entry;
	}
	
	std::string get_sprite_path(std::string sprite_name)
	{
		std::string sprite_path;
		
		std::string db_path = make_db_path("sprites");
		SQLite::Database db(db_path, SQLite::OPEN_READWRITE);
		SQLite::Statement query(db, "SELECT name, path FROM sprites WHERE name = ?");
		query.bind(1, sprite_name);
		
		if (query.executeStep())
		{
			sprite_path = query.getColumn(1).getString();
		}
		else
		{
			assert("Failed to retrieve sprite path from database");
		}
		
		return sprite_path;
	}
}
