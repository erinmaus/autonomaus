// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/utility.hpp"

bool autonomaus::fuzzy_string_match(
	const std::string& input,
	const std::string& value,
	int flags)
{
	if (value.empty())
	{
		return false;
	}

	std::string a = input, b = value;
	if (flags & fuzzy_match_ignore_case)
	{
		for (std::size_t i = 0; i < a.length(); ++i)
		{
			a[i] = std::tolower(a[i]);
		}

		for (std::size_t i = 0; i < b.length(); ++i)
		{
			b[i] = std::tolower(b[i]);
		}
	}

	if (b[0] == '%' && (flags & fuzzy_match_pattern))
	{
		b = b.substr(1);

		boost::regex e(b);
		if (boost::regex_match(a, e))
		{
			return true;
		}
	}

	if (flags & fuzzy_match_min)
	{
		if (std::strncmp(a.c_str(), b.c_str(), b.length()) == 0)
		{
			return true;
		}
	}

	return a == b;
}

// https://stackoverflow.com/a/217605
static inline void ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}

static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

std::string autonomaus::trim_string(const std::string& input)
{
	std::string result(input);
	trim(result);

	return result;
}

std::vector<std::string> autonomaus::split_string(
	const std::string& input,
	const std::string& delimiter)
{
	std::vector<std::string> result;
	std::size_t previous_position = 0;
	std::size_t position = input.find(delimiter);

	while (position != std::string::npos)
	{
		result.push_back(input.substr(previous_position, position - previous_position));

		previous_position = position + delimiter.length();
		position = input.find(delimiter, previous_position);
	}

	result.push_back(input.substr(previous_position));

	return result;
}
