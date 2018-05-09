// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include <cstring>
#include <sqlite3.h>
#include "statement.hpp"

twoflower::Brochure::Statement::Statement(sqlite3* database, const std::string& query) :
	database(database)
{
	sqlite3_stmt* s;
	auto result = sqlite3_prepare(database, query.c_str(), query.length(), &s, nullptr);
	if (result != SQLITE_OK)
	{
		throw std::runtime_error(sqlite3_errmsg(database));
	}

	statement = std::shared_ptr<sqlite3_stmt>(s, [](auto s) { sqlite3_finalize(s); });
}

twoflower::Brochure::Statement::~Statement()
{
	// Nothing.
}

void twoflower::Brochure::Statement::bind(const std::string& parameter, int value)
{
	bind(sqlite3_bind_parameter_index(statement.get(), parameter.c_str()), value);
}

void twoflower::Brochure::Statement::bind(int parameter, int value)
{
	if (parameter == 0)
	{
		return;
	}

	auto result = sqlite3_bind_int(statement.get(), parameter, value);
	if (result != SQLITE_OK)
	{
		throw std::runtime_error(sqlite3_errmsg(database));
	}
}

void twoflower::Brochure::Statement::bind(const std::string& parameter, float value)
{
	bind(sqlite3_bind_parameter_index(statement.get(), parameter.c_str()), value);
}

void twoflower::Brochure::Statement::bind(int parameter, float value)
{
	if (parameter == 0)
	{
		return;
	}

	auto result = sqlite3_bind_double(statement.get(), parameter, value);
	if (result != SQLITE_OK)
	{
		throw std::runtime_error(sqlite3_errmsg(database));
	}
}

void twoflower::Brochure::Statement::bind(const std::string& parameter, const std::string& value)
{
	bind(sqlite3_bind_parameter_index(statement.get(), parameter.c_str()), value);
}

void twoflower::Brochure::Statement::bind(int parameter, const std::string& value)
{
	if (parameter == 0)
	{
		return;
	}

	auto result = sqlite3_bind_text(
		statement.get(), parameter,
		value.c_str(), value.length(),
		SQLITE_TRANSIENT);
	if (result != SQLITE_OK)
	{
		throw std::runtime_error(sqlite3_errmsg(database));
	}
}

void twoflower::Brochure::Statement::bind(const std::string& parameter, const std::vector<std::uint8_t>& value)
{
	bind(sqlite3_bind_parameter_index(statement.get(), parameter.c_str()), value);
}

void twoflower::Brochure::Statement::bind(int parameter, const std::vector<std::uint8_t>& value)
{
	if (parameter == 0)
	{
		return;
	}

	auto result = sqlite3_bind_blob(
		statement.get(), parameter,
		&value[0], value.size(),
		SQLITE_TRANSIENT);
	if (result != SQLITE_OK)
	{
		throw std::runtime_error(sqlite3_errmsg(database));
	}
}

int twoflower::Brochure::Statement::execute()
{
	int count = 0;
	while (next())
	{
		++count;
	}

	return count;
}

bool twoflower::Brochure::Statement::next()
{
	if (is_done)
	{
		sqlite3_reset(statement.get());
		is_done = false;
	}

	auto result = sqlite3_step(statement.get());
	if (result == SQLITE_DONE)
	{
		is_done = true;
		return false;
	}
	else if (result != SQLITE_ROW)
	{
		throw std::runtime_error(sqlite3_errmsg(database));
	}

	return true;
}

void twoflower::Brochure::Statement::get(const std::string& column, int& value)
{
	return get(get_column_index(column), value);
}

void twoflower::Brochure::Statement::get(int column, int& value)
{
	value = sqlite3_column_int(statement.get(), column);
}

void twoflower::Brochure::Statement::get(const std::string& column, float& value)
{
	return get(get_column_index(column), value);
}

void twoflower::Brochure::Statement::get(int column, float& value)
{
	value = sqlite3_column_double(statement.get(), column);
}

void twoflower::Brochure::Statement::get(const std::string& column, std::string& value)
{
	return get(get_column_index(column), value);
}

void twoflower::Brochure::Statement::get(int column, std::string& value)
{
	auto result = sqlite3_column_text(statement.get(), column);
	if (result == nullptr)
	{
		value = std::string();
	}
	else
	{
		value = (const char*)result;
	}
}

void twoflower::Brochure::Statement::get(const std::string& column, std::vector<std::uint8_t>& value)
{
	return get(get_column_index(column), value);
}

void twoflower::Brochure::Statement::get(int column, std::vector<std::uint8_t>& value)
{
	auto result = sqlite3_column_blob(statement.get(), column);
	if (result == nullptr)
	{
		value = std::vector<std::uint8_t>();
	}
	else
	{
		std::size_t num_bytes = sqlite3_column_bytes(statement.get(), column);
		value.resize(num_bytes);
		std::memcpy(&value[0], result, num_bytes);
	}
}

bool twoflower::Brochure::Statement::empty(const std::string& column)
{
	return empty(get_column_index(column));
}

bool twoflower::Brochure::Statement::empty(int column)
{
	return sqlite3_column_type(statement.get(), column) == SQLITE_NULL;
}

int twoflower::Brochure::Statement::get_column_index(const std::string& column)
{
	for (int i = 0; i < sqlite3_column_count(statement.get()); ++i)
	{
		auto name = sqlite3_column_name(statement.get(), i);
		if (name != nullptr && column == name)
		{
			return i;
		}
	}

	return -1;
}
