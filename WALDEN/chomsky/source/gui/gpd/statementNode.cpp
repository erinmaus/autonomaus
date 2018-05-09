// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/groupStatementNode.hpp"
#include "chomsky/gui/gpd/matchStatementNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/printStatementNode.hpp"
#include "chomsky/gui/gpd/statementNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"

void chomsky::gpd::StatementNode::parse(Tokenizer& tokenizer)
{
	this->child = nullptr;

	auto current = tokenizer.peek();
	if (current.type == Token::type_left_curly)
	{
		this->child = add_child<GroupStatementNode>();
	}
	else if (current.type == Token::type_identifier &&
		MatchStatementNode::match(current.value))
	{
		this->child = add_child<MatchStatementNode>();
	}
	else if (current.type == Token::type_identifier &&
		current.value == "print")
	{
		this->child = add_child<PrintStatementNode>();
	}
	else
	{
		throw ParsingError(current, "expected group, match, or print statement");
	}

	assert(this->child != nullptr);
	this->child->parse(tokenizer);

	tokenizer.match(Token::type_semicolon);
}

void chomsky::gpd::StatementNode::resolve(Compiler& compiler) const
{
	this->child->resolve(compiler);
}

void chomsky::gpd::StatementNode::compile(Compiler& compiler) const
{
	this->child->compile(compiler);
}
