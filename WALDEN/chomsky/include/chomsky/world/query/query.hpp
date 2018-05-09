// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_QUERY_QUERY_HPP
#define CHOMSKY_WORLD_QUERY_QUERY_HPP

#include <string>

namespace chomsky
{
	template <typename Match>
	class Query
	{
	public:
		Query() = default;
		Query(const Match& match, int tag);
		~Query() = default;

		const Match& get_match() const;
		int get_tag() const;

		const std::string& get_category() const;
		void set_category(const std::string& value);
		bool has_category() const;

		enum
		{
			hint_ignore = 0,
			hint_suggest,
			hint_require
		};
		int get_hint() const;
		void set_hint(int value);

		const std::string& get_hint_value() const;
		void set_hint_value(const std::string& value);

	private:
		Match match;
		int tag;

		std::string category;

		int hint = hint_ignore;
		std::string hint_value;
	};
}

template <typename Match>
chomsky::Query<Match>::Query(const Match& match, int tag)
{
	this->match = match;
	this->tag = tag;
}

template <typename Match>
const Match& chomsky::Query<Match>::get_match() const
{
	return this->match;
}

template <typename Match>
int chomsky::Query<Match>::get_tag() const
{
	return this->tag;
}

template <typename Match>
const std::string& chomsky::Query<Match>::get_category() const
{
	return this->category;
}

template <typename Match>
void chomsky::Query<Match>::set_category(const std::string& value)
{
	this->category = value;
}

template <typename Match>
bool chomsky::Query<Match>::has_category() const
{
	return !this->category.empty();
}

template <typename Match>
int chomsky::Query<Match>::get_hint() const
{
	return this->hint;
}

template <typename Match>
void chomsky::Query<Match>::set_hint(int value)
{
	this->hint = value;
}

template <typename Match>
const std::string& chomsky::Query<Match>::get_hint_value() const
{
	return this->hint_value;
}

template <typename Match>
void chomsky::Query<Match>::set_hint_value(const std::string& value)
{
	this->hint_value = value;
}

#endif
