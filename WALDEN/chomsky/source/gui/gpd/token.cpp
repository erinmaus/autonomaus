// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "chomsky/gui/gpd/token.hpp"

std::string chomsky::gpd::Token::stringify_type(int type)
{
	switch (type)
	{
		case type_none:
			return "nothing";
		case type_identifier:
			return "identifier";
		case type_number_literal:
			return "number literal";
		case type_color_literal:
			return "color literal";
		case type_left_parenthesis:
			return "'('";
		case type_right_parenthesis:
			return "')'";
		case type_left_curly:
			return "'{'";
		case type_right_curly:
			return "'}'";
		case type_left_bracket:
			return "'['";
		case type_right_bracket:
			return "']'";
		case type_field_prefix:
			return "'.'";
		case type_comma:
			return "','";
		case type_colon:
			return "':'";
		case type_semicolon:
			return "';'";
		case type_wildcard:
			return "'*'";
		case type_end_of_file:
			return "end-of-file";
		default:
			assert(false);
			return "(unknown)";
	}
}
