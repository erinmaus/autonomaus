// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/findDecorator.hpp"

bool chomsky::parser::FindDecorator::process(
	GUIParser& parser,
	GUIElementStream& stream,
	GUIStateBuilder& state_builder) const
{
	auto position = stream.tell();

	if (back() != nullptr)
	{
		while (!stream.is_end_of_stream())
		{
			auto p = stream.tell();

			state_builder.enter();
			if (back()->process(parser, stream, state_builder))
			{
				state_builder.discard();
				stream.seek(p);
				return true;
			}
			state_builder.discard();

			stream.seek(p + 1);
		}
	}

	stream.seek(position);
	return false;
}
