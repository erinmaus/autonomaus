// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstdlib>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/widgetMatchStatementNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/tagCollectionNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"
#include "chomsky/gui/parser/group.hpp"
#include "chomsky/gui/parser/widget.hpp"

void chomsky::gpd::WidgetMatchStatementNode::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_identifier, "widget");
	if (tokenizer.peek().type == Token::type_wildcard)
	{
		tokenizer.match(Token::type_wildcard);
		this->widget.clear();
	}
	else
	{
		this->widget = tokenizer.match(Token::type_identifier).value;
	}

	if (tokenizer.peek().type == Token::type_left_bracket)
	{
		this->tags = add_child<TagCollectionNode>();
		this->tags->parse(tokenizer);
	}
	else
	{
		this->tags = nullptr;
	}
}

void chomsky::gpd::WidgetMatchStatementNode::resolve(Compiler& compiler) const
{
	// Nothing.
}

void chomsky::gpd::WidgetMatchStatementNode::compile(Compiler& compiler) const
{
	std::set<std::string> t;
	if (this->tags != nullptr)
	{
		for (auto i = this->tags->tags_begin(); i != this->tags->tags_end(); ++i)
		{
			t.insert(*i);
		}
	}

	auto node = new parser::Group();
	node->set_qualifier(parser::Group::qualifier_any);

	compiler.for_each_widget(this->widget, t, 
		[node](parser::Widget* match)
		{
			node->add(match);
		});

	compiler.add_node(node);
	compiler.top_node()->add(node);
}
