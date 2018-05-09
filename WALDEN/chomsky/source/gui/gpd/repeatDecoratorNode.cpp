// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/parser/repeatDecorator.hpp"
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/expressionNode.hpp"
#include "chomsky/gui/gpd/parameterListNode.hpp"
#include "chomsky/gui/gpd/repeatDecoratorNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"

void chomsky::gpd::RepeatDecoratorNode::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_identifier, "repeat");

	if (tokenizer.peek().type == Token::type_left_parenthesis)
	{
		this->parameters = add_child<ParameterListNode>();
		this->parameters->parse(tokenizer);
	}
	else
	{
		this->parameters = nullptr;
	}

	this->child = add_child<ExpressionNode>();
	this->child->parse(tokenizer);
}

void chomsky::gpd::RepeatDecoratorNode::resolve(Compiler& compiler) const
{
	this->parameters->resolve(compiler);
	this->child->resolve(compiler);
}

void chomsky::gpd::RepeatDecoratorNode::compile(Compiler& compiler) const
{
	auto decorator = new parser::RepeatDecorator();

	if (this->parameters != nullptr)
	{
		int value;
		if (this->parameters->try_get("iterations", value))
		{
			decorator->set_iterations(value);
		}
		else
		{
			if (this->parameters->try_get("min", value))
			{
				decorator->set_min_iterations(value);
			}

			if (this->parameters->try_get("max", value))
			{
				decorator->set_max_iterations(value);
			}
		}
	}

	compiler.add_node(decorator);
	compiler.top_node()->add(decorator);
	compiler.push_node(decorator);

	this->child->compile(compiler);

	compiler.pop_node();
}
