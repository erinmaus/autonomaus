// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <sstream>
#include "chomsky/gui/gpd/token.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"

chomsky::gpd::ParsingError::ParsingError(const Token& token, const std::string& what) :
	std::runtime_error(build_message(token.filename, token.line_num, token.column_num, what))
{
	// Nothing.
}

chomsky::gpd::ParsingError::ParsingError(
	const std::string& filename,
	int line_num, int column_num,
	const std::string& what) :
		std::runtime_error(build_message(filename, line_num, column_num, what))
{
	// Nothing.
}

std::string chomsky::gpd::ParsingError::build_message(
	const std::string& filename,
	int line_num, int column_num,
	const std::string& what)
{
	std::ostringstream message;
	message << filename << ":" << line_num << ":" << column_num << ": " << what;

	return message.str();
}
