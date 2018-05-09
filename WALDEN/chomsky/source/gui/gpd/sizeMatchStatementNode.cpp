// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstdlib>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/sizeMatchStatementNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/tagCollectionNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"
#include "chomsky/gui/parser/matchSize.hpp"

void chomsky::gpd::SizeMatchStatementNode::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_identifier, "size");

	if (tokenizer.peek().type == Token::type_wildcard)
	{
		tokenizer.match();
		this->width = 0;
	}
	else
	{
		std::string value = tokenizer.match(Token::type_number_literal).value;
		this->width = std::atoi(value.c_str());
	}

	if (tokenizer.peek().type == Token::type_wildcard)
	{
		tokenizer.match();
		this->height = 0;
	}
	else
	{
		std::string value = tokenizer.match(Token::type_number_literal).value;
		this->height = std::atoi(value.c_str());
	}
}

void chomsky::gpd::SizeMatchStatementNode::resolve(Compiler& compiler) const
{
	// Nothing.
}

void chomsky::gpd::SizeMatchStatementNode::compile(Compiler& compiler) const
{
	auto node = new parser::MatchSize();
	node->set_width(this->width);
	node->set_height(this->height);

	compiler.add_node(node);
	compiler.top_node()->add(node);
}
