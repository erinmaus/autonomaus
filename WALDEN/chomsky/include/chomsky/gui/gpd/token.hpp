// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_TOKEN_HPP
#define CHOMSKY_GUI_GPD_TOKEN_HPP

#include <string>

namespace chomsky { namespace gpd
{
	struct Token
	{
		enum
		{
			type_none = 0,
			type_identifier,
			type_number_literal,
			type_color_literal,
			type_left_parenthesis,
			type_right_parenthesis,
			type_left_curly,
			type_right_curly,
			type_left_bracket,
			type_right_bracket,
			type_field_prefix,
			type_comma,
			type_colon,
			type_semicolon,
			type_wildcard,
			type_end_of_file,
			type_max
		};

		int type;
		std::string value;
		std::string filename;
		int line_num, column_num;

		static std::string stringify_type(int type);
	};
} }

#endif
