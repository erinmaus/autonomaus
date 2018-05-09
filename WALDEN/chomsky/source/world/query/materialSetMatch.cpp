// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/query/materialSetMatch.hpp"

void chomsky::MaterialSetMatch::add(const MaterialDefinition& definition)
{
	this->definitions.push_back(definition);
}

const chomsky::MaterialDefinition& chomsky::MaterialSetMatch::get(
	std::size_t index) const
{
	return this->definitions.at(index);
}

void chomsky::MaterialSetMatch::remove(std::size_t index)
{
	this->definitions.erase(this->definitions.begin() + index);
}

std::size_t chomsky::MaterialSetMatch::count() const
{
	return this->definitions.size();
}

bool chomsky::MaterialSetMatch::empty() const
{
	return this->definitions.empty();
}

chomsky::MaterialSetMatch::iterator
chomsky::MaterialSetMatch::begin()
{
	return this->definitions.begin();
}

chomsky::MaterialSetMatch::const_iterator
chomsky::MaterialSetMatch::begin() const
{
	return this->definitions.begin();
}

chomsky::MaterialSetMatch::iterator
chomsky::MaterialSetMatch::end()
{
	return this->definitions.end();
}

chomsky::MaterialSetMatch::const_iterator
chomsky::MaterialSetMatch::end() const
{
	return this->definitions.end();
}
