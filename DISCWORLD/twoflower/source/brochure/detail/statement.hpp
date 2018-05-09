// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef TWOFLOWER_DETAIL_BROCHURE_STATEMENT_HPP
#define TWOFLOWER_DETAIL_BROCHURE_STATEMENT_HPP

#include <cstdint>
#include <vector>
#include <sqlite3.h>
#include "twoflower/brochure.hpp"

namespace twoflower
{
	struct Brochure::Statement
	{
	public:
		Statement(sqlite3* database, const std::string& query);
		~Statement();

		void bind(const std::string& parameter, int value);
		void bind(int parameter, int value);
		void bind(const std::string& parameter, float value);
		void bind(int parameter, float value);
		void bind(const std::string& parameter, const std::string& value);
		void bind(int parameter, const std::string& value);
		void bind(const std::string& parameter, const std::vector<std::uint8_t>& value);
		void bind(int parameter, const std::vector<std::uint8_t>& value);

		int execute();
		bool next();

		void get(const std::string& column, float& value);
		void get(int column, float& value);
		void get(const std::string& column, int& value);
		void get(int column, int& value);
		void get(const std::string& column, std::string& value);
		void get(int column, std::string& value);
		void get(const std::string& column, std::vector<std::uint8_t>& value);
		void get(int column, std::vector<std::uint8_t>& value);
		bool empty(const std::string& column);
		bool empty(int column);

	private:
		int get_column_index(const std::string& name);

		sqlite3* database;
		std::shared_ptr<sqlite3_stmt> statement;
		bool is_done = false;
	};
}

#endif
