// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include <stdexcept>
#include <sqlite3.h>
#include "database.hpp"
#include "statement.hpp"
#include "table.hpp"

twoflower::Brochure::Database::Database(const std::string& filename)
{
	auto result = sqlite3_open(filename.c_str(), &database);
	if (result != SQLITE_OK)
	{
		throw std::runtime_error(sqlite3_errmsg(database));
	}

	create_statement("PRAGMA foreign_keys=ON;").execute();
	create_statement("PRAGMA recursive_triggers=ON;").execute();
	create_statement("PRAGMA synchronous = OFF;").execute();
}

twoflower::Brochure::Database::~Database()
{
	sqlite3_close(database);
}

twoflower::Brochure::Statement
twoflower::Brochure::Database::create_statement(const std::string& query)
{
	return Statement(database, query);
}
