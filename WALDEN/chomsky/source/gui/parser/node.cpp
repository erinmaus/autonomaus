// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/parser/node.hpp"

void chomsky::parser::Node::add(const Node* node)
{
	this->children.push_back(node);
}

const chomsky::parser::Node* chomsky::parser::Node::back() const
{
	if (this->children.empty())
	{
		return nullptr;
	}

	return this->children.back();
}

chomsky::parser::Node::const_iterator chomsky::parser::Node::begin() const
{
	return this->children.begin();
}

chomsky::parser::Node::const_iterator chomsky::parser::Node::end() const
{
	return this->children.end();
}
