// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <string>
#include "chomsky/gui/gpd/decoratorNode.hpp"
#include "chomsky/gui/gpd/expressionNode.hpp"
#include "chomsky/gui/gpd/findDecoratorNode.hpp"
#include "chomsky/gui/gpd/insideDecoratorNode.hpp"
#include "chomsky/gui/gpd/invertDecoratorNode.hpp"
#include "chomsky/gui/gpd/optionalDecoratorNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/peekDecoratorNode.hpp"
#include "chomsky/gui/gpd/repeatDecoratorNode.hpp"
#include "chomsky/gui/gpd/rewindDecoratorNode.hpp"
#include "chomsky/gui/gpd/skipDecoratorNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"

void chomsky::gpd::DecoratorNode::parse(Tokenizer& tokenizer)
{
	auto current = tokenizer.peek();
	if (current.type != Token::type_identifier)
	{
		throw ParsingError(current, "expected decorator or statement");
	}

	this->child = nullptr;
	if (current.value == "find")
	{
		this->child = add_child<FindDecoratorNode>();
	}
	else if (current.value == "repeat")
	{
		this->child = add_child<RepeatDecoratorNode>();
	}
	else if (current.value == "optional")
	{
		this->child = add_child<OptionalDecoratorNode>();
	}
	else if (current.value == "skip")
	{
		this->child = add_child<SkipDecoratorNode>();
	}
	else if (current.value == "peek")
	{
		this->child = add_child<PeekDecoratorNode>();
	}
	else if (current.value == "invert")
	{
		this->child = add_child<InvertDecoratorNode>();
	}
	else if (current.value == "rewind")
	{
		this->child = add_child<RewindDecoratorNode>();
	}
	else if (current.value == "inside")
	{
		this->child = add_child<InsideDecoratorNode>();
	}
	else
	{
		this->child = add_child<ExpressionNode>();
	}

	assert(this->child != nullptr);
	this->child->parse(tokenizer);
}

void chomsky::gpd::DecoratorNode::resolve(Compiler& compiler) const
{
	this->child->resolve(compiler);
}

void chomsky::gpd::DecoratorNode::compile(Compiler& compiler) const
{
	this->child->compile(compiler);
}

bool chomsky::gpd::DecoratorNode::match(const std::string& identifier)
{
	if (identifier == "find" ||
		identifier == "repeat" ||
		identifier == "optional" ||
		identifier == "skip" ||
		identifier == "peek" ||
		identifier == "invert" ||
		identifier == "rewind" ||
		identifier == "inside")
	{
		return true;
	}

	return false;
}
