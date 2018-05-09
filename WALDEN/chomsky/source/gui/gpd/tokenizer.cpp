// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cctype>
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"

chomsky::gpd::Tokenizer::Tokenizer(
	const std::string& filename,
	const std::string& source)
{
	this->filename = filename;
	this->source = source;

	next();
}

bool chomsky::gpd::Tokenizer::is_end_of_file() const
{
	assert(this->offset <= this->source.size());
	return this->offset == this->source.size();
}

chomsky::gpd::Token chomsky::gpd::Tokenizer::peek() const
{
	return current;
}

chomsky::gpd::Token chomsky::gpd::Tokenizer::match()
{
	auto result = this->current;
	next();

	return result;
}

chomsky::gpd::Token chomsky::gpd::Tokenizer::match(int type)
{
	if (this->current.type != type)
	{
		std::string message =
			"expected " + Token::stringify_type(type) +
			", got " + Token::stringify_type(this->current.type);
		throw ParsingError(this->filename, this->line_num, this->column_num, message);
	}

	auto result = this->current;
	next();

	return result;
}

chomsky::gpd::Token chomsky::gpd::Tokenizer::match(int type, const std::string& value)
{
	auto result = match(type);
	if (result.value != value)
	{
		std::string message = "expected value '" + value + "', got '" + result.value + "'";
		throw ParsingError(this->filename, this->line_num, this->column_num, message);
	}

	return result;
}

void chomsky::gpd::Tokenizer::next()
{
	skip_whitespace();

	this->current = Token();
	this->current.filename = this->filename;
	this->current.line_num = this->line_num;
	this->current.column_num = this->column_num;
	if (!is_end_of_file())
	{
		char c = this->source[this->offset];
		if (std::isalpha(c))
		{
			this->current.type = Token::type_identifier;
			this->current.value = collect(
				[](char c)
				{
					return std::isalnum(c) || c == '-' || c == '_';
				});
		}
		else if (std::isdigit(c))
		{
			this->current.type = Token::type_number_literal;
			this->current.value = collect(
				[](char c)
				{
					return std::isdigit(c);
				});
		}
		else if (c == '#')
		{
			this->current.type = Token::type_color_literal;
			++this->offset;
			this->current.value = collect(
				[](char c)
				{
					c = std::tolower(c);
					return std::isdigit(c) || (c >= 'a' && c <= 'f');
				});
		}
		else
		{
			switch (c)
			{
				case '(':
					this->current.type = Token::type_left_parenthesis;
					break;
				case ')':
					this->current.type = Token::type_right_parenthesis;
					break;
				case '{':
					this->current.type = Token::type_left_curly;
					break;
				case '}':
					this->current.type = Token::type_right_curly;
					break;
				case '[':
					this->current.type = Token::type_left_bracket;
					break;
				case ']':
					this->current.type = Token::type_right_bracket;
					break;
				case '.':
					this->current.type = Token::type_field_prefix;
					break;
				case ',':
					this->current.type = Token::type_comma;
					break;
				case ':':
					this->current.type = Token::type_colon;
					break;
				case ';':
					this->current.type = Token::type_semicolon;
					break;
				case '*':
					this->current.type = Token::type_wildcard;
					break;
				default:
					throw ParsingError(
						this->filename, this->line_num, this->column_num,
						"unexpected token");
			}

			this->current.value = c;
			++this->offset;
			++this->column_num;
		}
	}
	else
	{
		this->current.type = Token::type_end_of_file;
	}
}

void chomsky::gpd::Tokenizer::skip_whitespace()
{
	while (!is_end_of_file() && std::isspace(this->source[this->offset]))
	{
		if (this->source[this->offset] == '\n')
		{
			++this->line_num;
			this->column_num = 1;
		}
		else
		{
			++this->column_num;
		}

		++this->offset;
	}
}

std::string chomsky::gpd::Tokenizer::collect(
	const std::function<bool(char)>& filter)
{
	std::string result;
	while (!is_end_of_file() && filter(this->source[this->offset]))
	{
		result += this->source[this->offset];
		++this->column_num;
		++this->offset;
	}

	return result;
}
