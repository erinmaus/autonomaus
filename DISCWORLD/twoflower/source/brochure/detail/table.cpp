// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include <sstream>
#include "database.hpp"
#include "statement.hpp"
#include "table.hpp"

const std::size_t twoflower::Brochure::Table::COLUMN_NAME;
const std::size_t twoflower::Brochure::Table::COLUMN_TYPE;
const std::size_t twoflower::Brochure::Table::COLUMN_NULLABLE;

twoflower::Brochure::Table::Table(const std::string& name) :
	name(name)
{
	// Nothing.
}

const std::string& twoflower::Brochure::Table::get_name() const
{
	return name;
}

void twoflower::Brochure::Table::add_primary_key(
	const std::string& name,
	Type type)
{
	primary_keys.emplace(name, type, false);
}

void twoflower::Brochure::Table::add_column(
	const std::string& name,
	Type type,
	bool nullable)
{
	columns.emplace(name, type, nullable);
}

void twoflower::Brochure::Table::bind_foreign_key(
	const std::string& other_table,
	const std::string& self_column,
	const std::string& reference_column)
{
	auto& foreign_key = foreign_keys[other_table];
	foreign_key[self_column] = reference_column;
}

bool twoflower::Brochure::Table::exists(Brochure::Database& database) const
{
	return table_exists(database) && match_columns(database) && match_foreign_keys(database);
}

void twoflower::Brochure::Table::create(Brochure::Database& database) const
{
	std::stringstream stream;
	stream << "CREATE TABLE" << " " << name << " " << "(" << std::endl;

	for (auto& primary_key: primary_keys)
	{
		auto name = std::get<COLUMN_NAME>(primary_key);
		auto type = std::get<COLUMN_TYPE>(primary_key);
		stream << "\t" << name << " " << get_type_literal(type) << " " << "NOT NULL";
		stream << "," << std::endl;
	}

	for (auto& column: columns)
	{
		auto name = std::get<COLUMN_NAME>(column);
		auto type = std::get<COLUMN_TYPE>(column);
		auto nullable = std::get<COLUMN_NULLABLE>(column);
		stream << "\t" << name << " " << get_type_literal(type);

		if (!nullable)
		{
			stream << " " << "NOT NULL";
		}

		if (type == Type::text)
		{
			stream << " " << "COLLATE NOCASE";
		}

		stream << "," << std::endl;
	}

	stream << "\t" << "PRIMARY KEY" << " " << "(";
	std::size_t index = 1;
	for (auto& primary_key: primary_keys)
	{
		auto name = std::get<COLUMN_NAME>(primary_key);
		stream << name;

		if (index < primary_keys.size())
		{
			stream << "," << " ";
		}
		++index;
	}
	stream << ")";

	if (!foreign_keys.empty())
	{
		stream << "," << std::endl;

		index = 1;
		for (auto& i: foreign_keys)
		{
			auto& foreign_table = i.first;
			auto& foreign_key = i.second;
			std::size_t reference_index;

			stream << "\t" << "FOREIGN KEY" << " " << "(";
			reference_index = 1;
			for (auto& reference: foreign_key)
			{
				stream << reference.first;

				if (reference_index < foreign_key.size())
				{
					stream << "," << " ";
				}
				++reference_index;
			}

			stream << ")" << " " << "REFERENCES" << " " << foreign_table << "(";
			reference_index = 1;
			for (auto& reference: foreign_key)
			{
				stream << reference.second;

				if (reference_index < foreign_key.size())
				{
					stream << "," << " ";
				}
				++reference_index;
			}

			stream << ")" << " " << "ON DELETE CASCADE";

			if (index < foreign_keys.size())
			{
				stream << ",";
			}
			++index;

			stream << std::endl;
		}
	}

	stream << std::endl;
	stream << ")" << ";";

	auto statement = database.create_statement(stream.str());
	statement.execute();
}

bool twoflower::Brochure::Table::table_exists(Brochure::Database& database) const
{
	auto statement = database.create_statement(
		"SELECT name FROM sqlite_master\n"
		"WHERE type='table' AND name=?");
	statement.bind(1, name);

	return statement.execute() == 1;
}

bool twoflower::Brochure::Table::match_foreign_keys(Brochure::Database& database) const
{
	std::stringstream query;
	query << "PRAGMA table_info(" << name << ");";
	auto statement = database.create_statement(query.str());

	Columns visited_primary_keys, visited_columns;
	while (statement.next())
	{
		std::string column_name;
		statement.get("name", column_name);

		std::string column_type_literal;
		statement.get("type", column_type_literal);
		Type column_type;
		if (column_type_literal == "INTEGER")
		{
			column_type = Type::integer;
		}
		else if (column_type_literal == "TEXT")
		{
			column_type = Type::text;
		}
		else if (column_type_literal == "REAL")
		{
			column_type = Type::real;
		}
		else if (column_type_literal == "BLOB")
		{
			column_type = Type::blob;
		}
		else
		{
			return false;
		}

		int column_not_null;
		statement.get("notnull", column_not_null);
		bool column_nullable = !column_not_null;

		int column_primary_key;
		statement.get("pk", column_primary_key);

		auto key = std::make_tuple(column_name, column_type, column_nullable);
		if (column_primary_key)
		{
			if (primary_keys.count(key) == 0)
			{
				return false;
			}

			visited_primary_keys.insert(key);
		}
		else
		{
			if (columns.count(key) == 0)
			{
				return false;
			}

			visited_columns.insert(key);
		}
	}

	for (auto& column: columns)
	{
		if (!visited_columns.count(column))
		{
			return false;
		}
	}

	for (auto& primary_key: primary_keys)
	{
		if (!visited_primary_keys.count(primary_key))
		{
			return false;
		}
	}

	return true;
}

bool twoflower::Brochure::Table::match_columns(Brochure::Database& database) const
{
	std::stringstream query;
	query << "PRAGMA foreign_key_list(" << name << ");";
	auto statement = database.create_statement(query.str());

	ForeignKeys visited_foreign_keys;
	while (statement.next())
	{
		std::string foreign_table;
		statement.get("table", foreign_table);

		std::string self_column;
		statement.get("from", self_column);

		std::string foreign_column;
		statement.get("to", foreign_column);

		visited_foreign_keys[foreign_table].emplace(self_column, foreign_column);
	}

	return visited_foreign_keys == foreign_keys;
}

std::string twoflower::Brochure::Table::get_type_literal(Type type)
{
	if (type == Type::integer)
	{
		return "INTEGER";
	}
	else if (type == Type::text)
	{
		return "TEXT";
	}
	else if (type == Type::real)
	{
		return "REAL";
	}
	else if (type == Type::blob)
	{
		return "BLOB";
	}
	else
	{
		// I don't know why having to add this makes me so angry.
		//
		// Don't pass garbage arguments, duh!
		throw std::runtime_error(
			"Type enumeration has invalid value;"
			" thus, no corresponding literal.");
	}
}
