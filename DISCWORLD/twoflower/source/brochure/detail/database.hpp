// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef TWOFLOWER_DETAIL_BROCHURE_DATABASE_HPP
#define TWOFLOWER_DETAIL_BROCHURE_DATABASE_HPP

#include <sqlite3.h>
#include "twoflower/brochure.hpp"

namespace twoflower
{
	class Brochure::Database
	{
	public:
		explicit Database(const std::string& filename);
		~Database();

		Statement create_statement(const std::string& query);
	private:
		sqlite3* database = nullptr;
	};
}

#endif
