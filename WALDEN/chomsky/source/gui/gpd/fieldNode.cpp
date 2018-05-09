// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/expressionNode.hpp"
#include "chomsky/gui/gpd/fieldNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"
#include "chomsky/gui/parser/field.hpp"

void chomsky::gpd::FieldNode::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_field_prefix);
	this->name = tokenizer.match(Token::type_identifier).value;

	if (tokenizer.peek().type == Token::type_left_bracket)
	{
		tokenizer.match(Token::type_left_bracket);
		tokenizer.match(Token::type_right_bracket);
		this->is_array = true;
	}
	else
	{
		this->is_array = false;
	}

	tokenizer.match(Token::type_colon);

	this->child = add_child<ExpressionNode>();
	this->child->parse(tokenizer);
}

void chomsky::gpd::FieldNode::resolve(Compiler& compiler) const
{
	this->child->resolve(compiler);
}

void chomsky::gpd::FieldNode::compile(Compiler& compiler) const
{
	auto node = new parser::Field();
	node->set_name(this->name);
	node->set_array(this->is_array);

	compiler.add_node(node);
	compiler.top_node()->add(node);
	compiler.push_node(node);

	this->child->compile(compiler);

	compiler.pop_node();
}
