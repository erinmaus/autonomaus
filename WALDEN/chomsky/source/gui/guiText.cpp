// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <algorithm>
#include <cctype>
#include "chomsky/gui/guiText.hpp"

const std::size_t chomsky::GUIText::SEGMENT_STRING;
const std::size_t chomsky::GUIText::SEGMENT_COLOR;

void chomsky::GUIText::add(const std::string& text, const glm::vec4& color)
{
	this->segments.emplace_back(text, color);
	this->is_string_dirty = true;
}

void chomsky::GUIText::remove(std::size_t index)
{
	this->segments.erase(this->segments.begin() + index);
	this->is_string_dirty = true;
}

chomsky::GUIText::Segment chomsky::GUIText::get(std::size_t index) const
{
	return this->segments.at(index);
}

std::size_t chomsky::GUIText::count() const
{
	return this->segments.size();
}

bool chomsky::GUIText::empty() const
{
	return this->segments.empty();
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

const std::string& chomsky::GUIText::to_string() const
{
	if (this->is_string_dirty)
	{
		this->string.clear();
		for (auto& segment: this->segments)
		{
			this->string += std::get<SEGMENT_STRING>(segment);
		}

		trim(this->string);
		this->is_string_dirty = false;
	}

	return this->string;
}

chomsky::GUIText::operator const std::string&() const
{
	return to_string();
}

chomsky::GUIText::const_iterator chomsky::GUIText::begin() const
{
	return this->segments.begin();
}

chomsky::GUIText::const_iterator chomsky::GUIText::end() const
{
	return this->segments.end();
}
