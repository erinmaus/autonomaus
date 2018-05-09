// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/matchImage.hpp"

void chomsky::parser::MatchImage::set_query(const std::string& value)
{
	this->query = value;
}

void chomsky::parser::MatchImage::set_wildcard()
{
	this->query = "*";
}

void chomsky::parser::MatchImage::add_query_tag(const std::string& value)
{
	this->tags.insert(value);
}

bool chomsky::parser::MatchImage::process(
	GUIParser& parser,
	GUIElementStream& stream,
	GUIStateBuilder& state_builder) const
{
	stream.push();

	GUIElement element;
	if (!stream.next(element))
	{
		stream.pop(false);
		return false;
	}

	if (!element.get_is_textured())
	{
		stream.pop(false);
		state_builder.set_failed(element);
		return false;
	}

	auto& dictionary = parser.get_texture_dictionary();
	if (dictionary.has(element.get_key()))
	{
		auto& entry = dictionary.get(element.get_key());
		if (this->query != "*" && entry.get_value() != this->query)
		{
			stream.pop(false);
			state_builder.set_failed(element);
			return false;
		}

		for (auto tag: this->tags)
		{
			if (!entry.has_tag(tag))
			{
				stream.pop(false);
				state_builder.set_failed(element);
				return false;
			}
		}
	}
	else
	{
		if (this->query != "*" || !this->tags.empty())
		{
			stream.pop(false);
			state_builder.set_failed(element);
			return false;
		}
	}

	state_builder.collect(element);
	stream.pop(true);
	return true;
}
