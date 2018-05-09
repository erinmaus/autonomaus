// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/widget.hpp"

void chomsky::parser::Widget::set_name(const std::string& value)
{
	this->name = value;
}

void chomsky::parser::Widget::add_tag(const std::string& tag)
{
	this->tags.insert(tag);
}

bool chomsky::parser::Widget::process(
	GUIParser& parser,
	GUIElementStream& stream,
	GUIStateBuilder& state_builder) const
{
	state_builder.make_widget(this->name);
	if (back() == nullptr || !back()->process(parser, stream, state_builder))
	{
		return false;
	}

	for (auto tag: this->tags)
	{
		state_builder.add_tag(tag);
	}

	return true;
}
