module;
#include "sqlitecpp/SQLiteCpp.h"
#include <iostream>
export module database;

export namespace database
{
	void print_db(std::wstring path);
	
	// paths are relative to the executable directory
	struct TileDefinition
	{
		std::string name;
		std::string path;
		std::uint8_t index = 0;
		std::uint8_t scale = 1;
	};
	
	void tiles_add_entry(TileDefinition entry);
	TileDefinition tiles_get_entry(std::string name);
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
		SQLite::Database db("database\\tiles.db", SQLite::OPEN_READWRITE);
		SQLite::Statement query(db, "INSERT INTO tiles (name, path, index, scale) VALUES (?, ?, ?, ?)");
		query.bind(1, entry.name);
		query.bind(2, entry.path);
		query.bind(3, static_cast<int>(entry.index));
		query.bind(4, static_cast<int>(entry.scale));
		
		query.exec();
	}
	
	TileDefinition tiles_get_entry(std::string name)
	{
		TileDefinition entry;
		
		try
		{
			SQLite::Database db("D:\\silmech\\workspaces\\forgette\\trunk\\build\\database\\tiles.db", SQLite::OPEN_READWRITE);
			SQLite::Statement query(db, "SELECT name, path, position, scale FROM tiles WHERE name = ?");
			query.bind(1, name);

			if (query.executeStep())
			{
				TileDefinition entry;
				entry.name = query.getColumn(0).getString();
				entry.path = query.getColumn(1).getString();
				entry.index = static_cast<std::uint8_t>(query.getColumn(2).getInt());
				entry.scale = static_cast<std::uint8_t>(query.getColumn(3).getInt());
				return entry;
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << "SQLite exception: " << e.what() << std::endl;
			assert(true);
		}
	    
	    return entry;
	}
}
