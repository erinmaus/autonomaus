// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_PARSING_ERROR_HPP
#define CHOMSKY_GUI_GPD_PARSING_ERROR_HPP

#include <stdexcept>
#include <string>

namespace chomsky { namespace gpd
{
	struct Token;

	class ParsingError : public std::runtime_error
	{
	public:
		ParsingError(const Token& token, const std::string& what);
		ParsingError(
			const std::string& filename,
			int line_num, int column_name,
			const std::string& what);

	private:
		std::string build_message(
			const std::string& filename,
			int line_num, int column_name,
			const std::string& what);
	};
} }

#endif
