// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/decoratorNode.hpp"
#include "chomsky/gui/gpd/expressionNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/statementNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"

void chomsky::gpd::ExpressionNode::parse(Tokenizer& tokenizer)
{
	auto current = tokenizer.peek();

	if (current.type == Token::type_identifier &&
		DecoratorNode::match(current.value))
	{
		this->child = add_child<DecoratorNode>();
	}
	else
	{
		this->child = add_child<StatementNode>();
	}

	assert(this->child != nullptr);
	this->child->parse(tokenizer);
}

void chomsky::gpd::ExpressionNode::resolve(Compiler& compiler) const
{
	this->child->resolve(compiler);
}

void chomsky::gpd::ExpressionNode::compile(Compiler& compiler) const
{
	this->child->compile(compiler);
}
