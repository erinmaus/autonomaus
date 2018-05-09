// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/matchColor.hpp"

void chomsky::parser::MatchColor::set_color(const glm::ivec4& value)
{
	this->color = value;
}

bool chomsky::parser::MatchColor::process(
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

	glm::ivec4 element_color = element.get_color() * 255.0f;
	if (element_color != this->color)
	{
		stream.pop(false);
		state_builder.set_failed(element);
		return false;
	}

	stream.pop(true);
	state_builder.collect(element);
	return true;
}
