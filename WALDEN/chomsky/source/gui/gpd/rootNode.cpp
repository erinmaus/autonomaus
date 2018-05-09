// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/gpd/rootNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"
#include "chomsky/gui/gpd/widgetNode.hpp"

void chomsky::gpd::RootNode::parse(Tokenizer& tokenizer)
{
	while (tokenizer.peek().type == Token::type_identifier)
	{
		auto widget = add_child<WidgetNode>();
		widget->parse(tokenizer);

		this->widgets.push_back(widget);
	}

	tokenizer.match(Token::type_end_of_file);
}

void chomsky::gpd::RootNode::resolve(Compiler& compiler) const
{
	for (auto& widget: this->widgets)
	{
		widget->resolve(compiler);
	}
}

void chomsky::gpd::RootNode::compile(Compiler& compiler) const
{
	for (auto& widget: this->widgets)
	{
		widget->compile(compiler);
	}
}
