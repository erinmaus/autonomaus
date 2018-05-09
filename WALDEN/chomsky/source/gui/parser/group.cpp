// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/group.hpp"

void chomsky::parser::Group::set_qualifier(int value)
{
	this->qualifier = value;
}

bool chomsky::parser::Group::process(
	GUIParser& parser,
	GUIElementStream& stream,
	GUIStateBuilder& state_builder) const
{
	stream.push();

	bool success = this->qualifier == qualifier_any ? false : true;
	for (auto child: *this)
	{
		if (this->qualifier == qualifier_any)
		{
			state_builder.enter();
		}
		bool s = child->process(parser, stream, state_builder);

		if (this->qualifier == qualifier_any)
		{
			if (s)
			{
				success = true;
				state_builder.replace();
				break;
			}
			else
			{
				state_builder.discard();
			}
		}
		else if (this->qualifier == qualifier_all && !s)
		{
			success = false;
			break;
		}
	}

	stream.pop(success);
	return success;
}
