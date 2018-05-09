// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/printStatementNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"
#include "chomsky/gui/parser/print.hpp"

void chomsky::gpd::PrintStatementNode::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_identifier, "print");
}

void chomsky::gpd::PrintStatementNode::resolve(Compiler& compiler) const
{
	// Nothing.
}

void chomsky::gpd::PrintStatementNode::compile(Compiler& compiler) const
{
	auto node = new parser::Print();
	compiler.add_node(node);
	compiler.top_node()->add(node);
}
