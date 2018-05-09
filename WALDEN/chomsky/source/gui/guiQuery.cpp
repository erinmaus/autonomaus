// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cctype>
#include <limits>
#include <sstream>
#include "chomsky/gui/guiQuery.hpp"

const std::size_t chomsky::GUIQuery::NONE = std::numeric_limits<std::size_t>::max();

chomsky::GUIQuery::GUIQuery(const std::string& path)
{
	try
	{
		parse(path);
	}
	catch (...)
	{
		this->query.clear();
		this->inputs.clear();

		throw;
	}
}

chomsky::GUIQuery::GUIQuery(const char* path)
{
	try
	{
		parse(path);
	}
	catch (...)
	{
		this->query.clear();
		this->inputs.clear();

		throw;
	}
}

bool chomsky::GUIQuery::valid() const
{
	return !this->query.empty();
}

chomsky::GUIQuery::operator bool() const
{
	return valid();
}

void chomsky::GUIQuery::set_input(std::size_t index, int value)
{
	if (index < this->inputs.size())
	{
		this->inputs[index] = std::to_string(value);
	}
}

void chomsky::GUIQuery::set_input(std::size_t index, const std::string& value)
{
	if (index < this->inputs.size())
	{
		this->inputs[index] = value;
	}
}

void chomsky::GUIQuery::unset_input(std::size_t index)
{
	if (index < this->inputs.size())
	{
		this->inputs[index] = "*";
	}
}

std::size_t chomsky::GUIQuery::get_num_inputs() const
{
	return this->inputs.size();
}

void chomsky::GUIQuery::parse(const std::string& path)
{
	auto begin = path.begin();
	auto end = path.end();

	if (begin == end)
	{
		throw std::runtime_error("empty path string");
	}

	auto current = begin;
	while (current != end)
	{
		if (*current == '*')
		{
			this->query.emplace_back(Match::match_field, "*");
			++current;
		}
		else if (*current == '?')
		{
			this->query.emplace_back(Match::match_field);
			this->query.back().name_input = this->inputs.size();
			this->inputs.emplace_back("*");
			++current;
		}
		else if (std::isalpha(*current) || *current == '-' || *current == '_')
		{ 
			std::string name;
			do
			{
				name += *current;
				++current;
			} while (is_identifier(*current) && current != end);

			this->query.emplace_back(Match::match_field, name);
		}
		else
		{
			std::stringstream message;
			message << "unexpected character '" << *current << "'"
				<< " at index " << current - begin;
			throw std::runtime_error(message.str());
		}

		if (*current == '[')
		{
			++current;

			if (current == end)
			{
				throw std::runtime_error("expected array input, got end-of-path");
			}

			this->query.emplace_back(Match::match_index, this->query.back().name);
			if (*current == '*')
			{
				this->query.back().index = "*";
				++current;
			}
			else if (*current == '?')
			{
				this->query.back().index_input = this->inputs.size();
				this->inputs.emplace_back("*");
				++current;
			}
			else if (std::isdigit(*current))
			{
				std::string value;
				do
				{
					value += *current;
					++current;
				} while (std::isdigit(*current) && current != end);

				this->query.back().index = value;
			}

			if (current == end || *current != ']')
			{
				std::stringstream message;
				message << "expected closing bracket ']' near " << current - begin;
				throw std::runtime_error(message.str());
			}

			++current;
		}

		if (*current == ':')
		{
			++current;

			if (current == end)
			{
				throw std::runtime_error("expected widget class name, got end-of-path");
			}

			if (*current == '*')
			{
				this->query.back().widget = "*";
				++current;
			}
			else if (*current == '?')
			{
				this->query.back().widget_input = this->inputs.size();
				this->inputs.emplace_back("*");
				++current;
			}
			else if (std::isalpha(*current) || *current == '-' || *current == '_')
			{ 
				std::string name;
				do
				{
					name += *current;
					++current;
				} while (is_identifier(*current) && current != end);

				this->query.back().widget = name;
			}
			else
			{
				std::stringstream message;
				message << "unexpected character '" << *current << "'"
					<< " at index " << current - begin;
				throw std::runtime_error(message.str());
			}
		}

		if (current != end)
		{
			if (*current != '.')
			{
				std::stringstream message;
				message << "unexpected character '" << *current << "'"
					<< " at index " << current - begin;
				throw std::runtime_error(message.str());
			}

			++current;
		}
	}
}

bool chomsky::GUIQuery::is_identifier(char c)
{
	return std::isalnum(c) || c == '-' || c == '_';
}

chomsky::GUIQuery::Match::Match(int type, std::string name)
{
	this->type = type;
	this->name = name;
}

bool chomsky::GUIQuery::collect(
	const GUIContent& content,
	std::vector<GUIContent>& result) const
{
	if (!valid())
	{
		return false;
	}

	auto name = get_match_name(0);
	auto widget = get_match_widget(0);
	if (widget != "*" && content.get_widget_class() != widget)
	{
		return false;
	}

	std::size_t old_size = result.size();
	if (name == "*" || name == content.get_name())
	{
		collect(content, 1, result);
	}

	return old_size != result.size();
}

void chomsky::GUIQuery::collect(
	const GUIContent& content,
	std::size_t current_index,
	std::vector<GUIContent>& result) const
{
	if (current_index >= this->query.size())
	{
		result.push_back(content);
		return;
	}

	auto next_index = current_index + 1;
	switch (this->query[current_index].type)
	{
		case Match::match_field:
			{
				auto name = get_match_name(current_index);
				auto widget = get_match_widget(current_index);
				if (name == "*")
				{
					for (auto& i: content)
					{
						auto& child = *i.second;
						if (widget == "*" || child.get_widget_class() == widget)
						{
							collect(child, next_index, result);
						}
					}
				}
				else if (content.has(name))
				{
					auto& child = content.get(name);
					if (widget == "*" || child.get_widget_class() == widget)
					{
						collect(content.get(name), next_index, result);
					}
				}
			}
			break;
		case Match::match_index:
			{
				auto name = get_match_name(current_index);
				auto index = get_match_index(current_index);
				auto widget = get_match_widget(current_index);

				if (index == "*")
				{
					std::size_t count = content.count(name);
					for (std::size_t i = 0; i < count; ++i)
					{
						auto& child = content.get(name, i);
						if (widget == "*" || child.get_widget_class() == widget)
						{
							collect(child, next_index, result);
						}
					}
				}
				else
				{
					std::size_t i = std::stoi(index);
					if (content.count(name) > i)
					{
						auto& child = content.get(name, i);
						if (widget == "*" || child.get_widget_class() == widget)
						{
							collect(child, next_index, result);
						}
					}
				}
			}
			break;
	}
}

std::string chomsky::GUIQuery::get_match_name(std::size_t index) const
{
	if (index >= this->query.size())
	{
		return std::string();
	}

	auto& match = this->query[index];
	if (match.name_input != NONE)
	{
		return get_input(match.name_input);
	}
	else
	{
		return match.name;
	}
}

std::string chomsky::GUIQuery::get_match_widget(std::size_t index) const
{
	if (index >= this->query.size())
	{
		return std::string();
	}

	auto& match = this->query[index];
	if (match.widget_input != NONE)
	{
		return get_input(match.widget_input);
	}
	else
	{
		return match.widget;
	}
}

std::string chomsky::GUIQuery::get_match_index(std::size_t index) const
{
	if (index >= this->query.size())
	{
		return std::string();
	}

	auto& match = this->query[index];
	if (match.index_input != NONE)
	{
		auto result = get_input(match.index_input);
		if (result.empty() || !std::isdigit(result[0]))
		{
			return "*";
		}
		else
		{
			return result;
		}
	}
	else
	{
		return match.index;
	}
}

std::string chomsky::GUIQuery::get_input(std::size_t index) const
{
	if (index >= this->inputs.size())
	{
		return std::string();
	}

	return this->inputs[index];
}
