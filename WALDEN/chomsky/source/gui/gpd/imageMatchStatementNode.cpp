// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstdlib>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/imageMatchStatementNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/tagCollectionNode.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"
#include "chomsky/gui/parser/matchImage.hpp"

void chomsky::gpd::ImageMatchStatementNode::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_identifier, "image");
	if (tokenizer.peek().type == Token::type_wildcard)
	{
		tokenizer.match(Token::type_wildcard);
		this->image.clear();
	}
	else
	{
		this->image = tokenizer.match(Token::type_identifier).value;
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

void chomsky::gpd::ImageMatchStatementNode::resolve(Compiler& compiler) const
{
	// Nothing.
}

void chomsky::gpd::ImageMatchStatementNode::compile(Compiler& compiler) const
{
	auto node = new parser::MatchImage();
	if (this->image.empty())
	{
		node->set_wildcard();
	}
	else
	{
		node->set_query(this->image);
	}

	if (this->tags != nullptr)
	{
		for (auto i = this->tags->tags_begin(); i != this->tags->tags_end(); ++i)
		{
			node->add_query_tag(*i);
		}
	}

	compiler.add_node(node);
	compiler.top_node()->add(node);
}
