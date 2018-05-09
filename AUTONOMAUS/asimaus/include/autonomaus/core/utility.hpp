// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_CORE_UTILITY_HPP
#define AUTONOMAUS_CORE_UTILITY_HPP

#include <cctype>
#include <string>
#include <boost/regex.hpp>

namespace autonomaus
{
	enum
	{
		fuzzy_match_pattern = 1 << 0,
		fuzzy_match_min = 1 << 1,
		fuzzy_match_ignore_case = 1 << 2,
		fuzzy_match_exact = 0,
		fuzzy_match_default = fuzzy_match_pattern | fuzzy_match_min | fuzzy_match_ignore_case
	};

	bool fuzzy_string_match(
		const std::string& input,
		const std::string& value,
		int flags = fuzzy_match_default);

	std::string trim_string(const std::string& value);

	std::vector<std::string> split_string(const std::string& value, const std::string& delim);
}

#endif
