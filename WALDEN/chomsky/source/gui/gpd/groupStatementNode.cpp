// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/groupStatementNode.hpp"
#include "chomsky/gui/gpd/fieldNode.hpp"
#include "chomsky/gui/gpd/expressionNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"
#include "chomsky/gui/parser/group.hpp"

void chomsky::gpd::GroupStatementNode::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_left_curly);
	do
	{
		Node* node;
		if (tokenizer.peek().type == Token::type_field_prefix)
		{
			node = add_child<FieldNode>();
		}
		else
		{
			node = add_child<ExpressionNode>();
		}

		node->parse(tokenizer);
		this->children.push_back(node);
	} while (tokenizer.peek().type != Token::type_right_curly);
	tokenizer.match(Token::type_right_curly);

	if (tokenizer.peek().type == Token::type_identifier)
	{
		auto token = tokenizer.match();
		if (token.value == "any")
		{
			this->qualifier = parser::Group::qualifier_any;
		}
		else if (token.value == "all")
		{
			this->qualifier = parser::Group::qualifier_all;
		}
		else
		{
			throw ParsingError(token, "invalid qualifier; expected 'any' or 'all'");
		}
	}
	else
	{
		this->qualifier = parser::Group::qualifier_all;
	}
}

void chomsky::gpd::GroupStatementNode::resolve(Compiler& compiler) const
{
	for (auto child: this->children)
	{
		child->resolve(compiler);
	}
}

void chomsky::gpd::GroupStatementNode::compile(Compiler& compiler) const
{
	auto node = new parser::Group();
	node->set_qualifier(this->qualifier);

	compiler.add_node(node);
	compiler.top_node()->add(node);
	compiler.push_node(node);

	for (auto child: this->children)
	{
		child->compile(compiler);
	}

	compiler.pop_node();
}
