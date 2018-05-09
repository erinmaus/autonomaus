// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/matchSize.hpp"

void chomsky::parser::MatchSize::set_width(int value)
{
	this->width = value;
}

void chomsky::parser::MatchSize::unset_width()
{
	this->width = 0;
}

void chomsky::parser::MatchSize::set_height(int value)
{
	this->height = value;
}

void chomsky::parser::MatchSize::unset_height()
{
	this->height = 0;
}

bool chomsky::parser::MatchSize::process(
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

	if (this->width > 0 && std::abs(element.get_width() - this->width) > 1)
	{
		stream.pop(false);
		state_builder.set_failed(element);
		return false;
	}

	if (this->height > 0 && std::abs(element.get_height() - this->height) > 1)
	{
		stream.pop(false);
		state_builder.set_failed(element);
		return false;
	}

	stream.pop(true);
	state_builder.collect(element);
	return true;
}
