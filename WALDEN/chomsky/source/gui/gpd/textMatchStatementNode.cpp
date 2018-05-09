// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstdlib>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/textMatchStatementNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/parameterListNode.hpp"
#include "chomsky/gui/gpd/tagCollectionNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"
#include "chomsky/gui/parser/matchText.hpp"

void chomsky::gpd::TextMatchStatementNode::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_identifier, "text");
	if (tokenizer.peek().type == Token::type_left_parenthesis)
	{
		this->parameters = add_child<ParameterListNode>();
		this->parameters->parse(tokenizer);
	}

	if (tokenizer.peek().type == Token::type_wildcard)
	{
		tokenizer.match(Token::type_wildcard);
		this->font.clear();
	}
	else
	{
		this->font = tokenizer.match(Token::type_identifier).value;
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

void chomsky::gpd::TextMatchStatementNode::resolve(Compiler& compiler) const
{
	// Nothing.
}

void chomsky::gpd::TextMatchStatementNode::compile(Compiler& compiler) const
{
	auto node = new parser::MatchText();
	if (this->font.empty())
	{
		node->set_wildcard();
	}
	else
	{
		node->set_font(this->font);
	}

	if (this->tags != nullptr)
	{
		for (auto i = this->tags->tags_begin(); i != this->tags->tags_end(); ++i)
		{
			node->add_font_tag(*i);
		}
	}

	if (this->parameters != nullptr)
	{
		bool shadowed;
		if (this->parameters->try_get("shadowed", shadowed))
		{
			node->set_shadowed(shadowed);
		}

		bool include_spaces;
		if (this->parameters->try_get("include_spaces", include_spaces))
		{
			node->set_include_spaces(include_spaces);
		}

		bool break_on_newline;
		if (this->parameters->try_get("break_on_newline", break_on_newline))
		{
			node->set_break_on_newline(break_on_newline);
		}

		bool break_on_color;
		if (this->parameters->try_get("break_on_color", break_on_color))
		{
			node->set_break_on_color(break_on_color);
		}
	}

	compiler.add_node(node);
	compiler.top_node()->add(node);
}
