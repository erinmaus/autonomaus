// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/expressionNode.hpp"
#include "chomsky/gui/gpd/tagCollectionNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"
#include "chomsky/gui/gpd/widgetNode.hpp"

void chomsky::gpd::WidgetNode::parse(Tokenizer& tokenizer)
{
	this->name = tokenizer.match(Token::type_identifier).value;

	if (tokenizer.peek().type == Token::type_left_bracket)
	{
		this->tags = add_child<TagCollectionNode>();
		this->tags->parse(tokenizer);
	}
	else
	{
		this->tags = nullptr;
	}

	tokenizer.match(Token::type_colon);

	this->expression = add_child<ExpressionNode>();
	this->expression->parse(tokenizer);

	this->widget = nullptr;
}

void chomsky::gpd::WidgetNode::resolve(Compiler& compiler) const
{
	this->widget = new parser::Widget();
	this->widget->set_name(this->name);

	std::set<std::string> t;
	if (this->tags != nullptr)
	{
		for (auto i = this->tags->tags_begin(); i != this->tags->tags_end(); ++i)
		{
			this->widget->add_tag(*i);
			t.insert(*i);
		}
	}

	compiler.add_node(this->widget);
	compiler.register_widget(this->widget, this->name, t);
}

void chomsky::gpd::WidgetNode::compile(Compiler& compiler) const
{
	compiler.push_node(this->widget);
	this->expression->compile(compiler);
	compiler.pop_node();
}
