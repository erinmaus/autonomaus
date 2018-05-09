// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstdlib>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/parameterListNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"

void chomsky::gpd::ParameterListNode::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_left_parenthesis);

	bool is_first = true;
	do
	{
		if (!is_first)
		{
			tokenizer.match(Token::type_comma);
		}
		else
		{
			is_first = false;
		}

		auto current = tokenizer.match(Token::type_identifier);
		auto name = current.value;
		tokenizer.match(Token::type_colon);
		auto token = tokenizer.peek();

		if (!(token.type == Token::type_identifier ||
			token.type == Token::type_number_literal ||
			token.type == Token::type_color_literal))
		{
			printf("%d\n", token.type);
			throw ParsingError(current, "expected identifier, number literal, or color literal");
		}
		else
		{
			tokenizer.match();
		}

		if (!this->parameters.emplace(name, token.value).second)
		{
			throw ParsingError(current, "duplicate parameter");
		}
	} while (tokenizer.peek().type != Token::type_right_parenthesis);
	tokenizer.match(Token::type_right_parenthesis);
}

void chomsky::gpd::ParameterListNode::resolve(Compiler& compiler) const
{
	// Nothing.
}

void chomsky::gpd::ParameterListNode::compile(Compiler& compiler) const
{
	// Nothing.
}

bool chomsky::gpd::ParameterListNode::try_get(
	const std::string& parameter, int& result) const
{
	auto iter = this->parameters.find(parameter);
	if (iter != this->parameters.end())
	{
		result = std::atoi(iter->second.c_str());
		return true;
	}

	return false;
}

bool chomsky::gpd::ParameterListNode::try_get(
	const std::string& parameter, bool& result) const
{
	auto iter = this->parameters.find(parameter);
	if (iter != this->parameters.end())
	{
		if (iter->second == "true")
		{
			result = true;
		}
		else if (iter->second == "false")
		{
			result = false;
		}
		else
		{
			return false;
		}

		return true;
	}

	return false;
}

bool chomsky::gpd::ParameterListNode::try_get(
	const std::string& parameter, std::string& result) const
{
	auto iter = this->parameters.find(parameter);
	if (iter != this->parameters.end())
	{
		result = iter->second;
		return true;
	}

	return false;
}
