// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_TOKENIZER_HPP
#define CHOMSKY_GUI_GPD_TOKENIZER_HPP

#include <functional>
#include <string>
#include "chomsky/gui/gpd/token.hpp"

namespace chomsky { namespace gpd
{
	class Tokenizer
	{
	public:
		Tokenizer(const std::string& filename, const std::string& source);
		~Tokenizer() = default;

		bool is_end_of_file() const;

		Token peek() const;
		Token match();
		Token match(int type);
		Token match(int type, const std::string& value);

	private:
		void next();
		void skip_whitespace();
		std::string collect(const std::function<bool(char)>& filter);

		std::string filename;
		std::string source;
		std::size_t offset = 0;
		int line_num = 1;
		int column_num = 1;

		Token current;
	};
} }

#endif
