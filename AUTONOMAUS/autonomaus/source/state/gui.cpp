// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/state/gui.hpp"

bool autonomaus::GUI::has(const std::string& name) const
{
	return this->contents.count(name) != 0 || this->compile_errors.count(name) != 0;
}

bool autonomaus::GUI::valid(const std::string& name) const
{
	return this->compile_errors.count(name) == 0;
}

const chomsky::GUIContent& autonomaus::GUI::get(const std::string& name) const
{
	if (!has(name))
	{
		return empty_content;
	}

	return this->contents.find(name)->second;
}

std::string autonomaus::GUI::get_error(const std::string& name)
{
	if (!valid(name))
	{
		return this->compile_errors.find(name)->second;
	}

	return std::string();
}

std::size_t autonomaus::GUI::get_num_elements() const
{
	return this->elements.size();
}

const chomsky::GUIElement& autonomaus::GUI::get_element(std::size_t index) const
{
	return this->elements.at(index);
}

autonomaus::GUI::const_iterator autonomaus::GUI::begin() const
{
	return this->contents.begin();
}

autonomaus::GUI::const_iterator autonomaus::GUI::end() const
{
	return this->contents.end();
}
