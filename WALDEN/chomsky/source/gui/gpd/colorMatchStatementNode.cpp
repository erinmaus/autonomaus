// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstdlib>
#include "chomsky/gui/gpd/colorMatchStatementNode.hpp"
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"
#include "chomsky/gui/parser/matchColor.hpp"

void chomsky::gpd::ColorMatchStatementNode::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_identifier, "color");
	auto token = tokenizer.match(Token::type_color_literal);
	if (token.value.size() != 8)
	{
		throw ParsingError(token, "expected RGBA (4-byte) color literal");
	}

	this->color.r = std::strtol(token.value.substr(0, 2).c_str(), nullptr, 16);
	this->color.g = std::strtol(token.value.substr(2, 2).c_str(), nullptr, 16);
	this->color.b = std::strtol(token.value.substr(4, 2).c_str(), nullptr, 16);
	this->color.a = std::strtol(token.value.substr(6, 2).c_str(), nullptr, 16);
}

void chomsky::gpd::ColorMatchStatementNode::resolve(Compiler& compiler) const
{
	// Nothing.
}

void chomsky::gpd::ColorMatchStatementNode::compile(Compiler& compiler) const
{
	auto node = new parser::MatchColor();
	node->set_color(this->color);

	compiler.add_node(node);
	compiler.top_node()->add(node);
}
