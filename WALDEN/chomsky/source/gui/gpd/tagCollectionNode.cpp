// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstdlib>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/tagCollectionNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"

void chomsky::gpd::TagCollectionNode::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_left_bracket);

	this->tags.insert(tokenizer.match(Token::type_identifier).value);
	while (tokenizer.peek().type != Token::type_right_bracket)
	{
		tokenizer.match(Token::type_comma);

		auto token = tokenizer.match(Token::type_identifier);
		if (!this->tags.insert(token.value).second)
		{
			throw ParsingError(token, "duplicate tag");
		}
	}

	tokenizer.match(Token::type_right_bracket);
}

void chomsky::gpd::TagCollectionNode::resolve(Compiler& compiler) const
{
	// Nothing.
}

void chomsky::gpd::TagCollectionNode::compile(Compiler& compiler) const
{
	// Nothing.
}

chomsky::gpd::TagCollectionNode::const_tag_iterator
chomsky::gpd::TagCollectionNode::tags_begin() const
{
	return this->tags.begin();
}

chomsky::gpd::TagCollectionNode::const_tag_iterator
chomsky::gpd::TagCollectionNode::tags_end() const
{
	return this->tags.end();
}
