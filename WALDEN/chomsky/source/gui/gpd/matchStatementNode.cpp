// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <string>
#include "chomsky/gui/gpd/colorMatchStatementNode.hpp"
#include "chomsky/gui/gpd/imageMatchStatementNode.hpp"
#include "chomsky/gui/gpd/widgetMatchStatementNode.hpp"
#include "chomsky/gui/gpd/matchStatementNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/sizeMatchStatementNode.hpp"
#include "chomsky/gui/gpd/textMatchStatementNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"

void chomsky::gpd::MatchStatementNode::parse(Tokenizer& tokenizer)
{
	auto current = tokenizer.peek();
	if (current.type != Token::type_identifier)
	{
		throw ParsingError(current, "expected decorator or statement");
	}

	this->child = nullptr;
	if (current.value == "color")
	{
		this->child = add_child<ColorMatchStatementNode>();
	}
	else if (current.value == "image")
	{
		this->child = add_child<ImageMatchStatementNode>();
	}
	else if (current.value == "text")
	{
		this->child = add_child<TextMatchStatementNode>();
	}
	else if (current.value == "size")
	{
		this->child = add_child<SizeMatchStatementNode>();
	}
	else if (current.value == "widget")
	{
		this->child = add_child<WidgetMatchStatementNode>();
	}
	else
	{
		assert(false);
		throw ParsingError(current, "unimplemented match statement");
	}

	assert(this->child != nullptr);
	this->child->parse(tokenizer);
}

void chomsky::gpd::MatchStatementNode::resolve(Compiler& compiler) const
{
	this->child->resolve(compiler);
}

void chomsky::gpd::MatchStatementNode::compile(Compiler& compiler) const
{
	this->child->compile(compiler);
}

bool chomsky::gpd::MatchStatementNode::match(const std::string& identifier)
{
	if (identifier == "color" ||
		identifier == "image" ||
		identifier == "text" ||
		identifier == "size" ||
		identifier == "widget")
	{
		return true;
	}

	return false;
}
