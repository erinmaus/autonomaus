// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiTags.hpp"

void chomsky::GUITags::add(const std::string& tag)
{
	this->tags.insert(tag);
}

void chomsky::GUITags::remove(const std::string& tag)
{
	this->tags.erase(tag);
}

bool chomsky::GUITags::has(const std::string& tag) const
{
	return this->tags.count(tag) != 0;
}

bool chomsky::GUITags::match(const GUITags& other) const
{
	for (auto tag: this->tags)
	{
		if (!other.has(tag))
		{
			return false;
		}
	}

	return true;
}

std::size_t chomsky::GUITags::count() const
{
	return this->tags.size();
}

bool chomsky::GUITags::empty() const
{
	return this->tags.empty();
}

chomsky::GUITags::const_iterator chomsky::GUITags::begin() const
{
	return this->tags.begin();
}

chomsky::GUITags::const_iterator chomsky::GUITags::end() const
{
	return this->tags.end();
}
