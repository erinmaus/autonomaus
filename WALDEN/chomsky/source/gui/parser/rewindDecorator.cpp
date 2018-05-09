// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/rewindDecorator.hpp"

bool chomsky::parser::RewindDecorator::process(
	GUIParser& parser,
	GUIElementStream& stream,
	GUIStateBuilder& state_builder) const
{
	if (back() != nullptr)
	{
		auto position = stream.tell();
		stream.rewind();

		bool success = back()->process(parser, stream, state_builder);
		stream.seek(position);

		return success;
	}

	return false;
}
