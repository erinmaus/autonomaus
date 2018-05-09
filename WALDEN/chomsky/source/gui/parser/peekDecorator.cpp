// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/peekDecorator.hpp"

bool chomsky::parser::PeekDecorator::process(
	GUIParser& parser,
	GUIElementStream& stream,
	GUIStateBuilder& state_builder) const
{
	if (back() != nullptr)
	{
		stream.push();
		state_builder.enter();
		bool success = back()->process(parser, stream, state_builder);
		state_builder.discard();
		stream.pop(false);

		return success;
	}

	return false;
}
