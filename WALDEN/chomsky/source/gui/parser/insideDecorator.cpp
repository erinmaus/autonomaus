// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/insideDecorator.hpp"

bool chomsky::parser::InsideDecorator::process(
	GUIParser& parser,
	GUIElementStream& stream,
	GUIStateBuilder& state_builder) const
{
	auto& parent = state_builder.get_parent();
	int parent_x1 = parent.get_x();
	int parent_x2 = parent.get_x() + parent.get_width();
	int parent_y1 = parent.get_y();
	int parent_y2 = parent.get_y() + parent.get_height();

	if (back() != nullptr)
	{
		stream.push();
		state_builder.enter(false);
		bool success = back()->process(parser, stream, state_builder);
		stream.pop(false);

		if (!success)
		{
			state_builder.discard();
			return false;
		}

		auto& child = state_builder.get_current();
		int child_x1 = child.get_x();
		int child_x2 = child.get_x() + child.get_width();
		int child_y1 = child.get_y();
		int child_y2 = child.get_y() + child.get_height();
		state_builder.discard();

		if (child_x1 >= parent_x2 || parent_x1 >= child_x2 ||
			child_y1 >= parent_y2 || parent_y1 >= child_y2)
		{
			return false;
		}

		return back()->process(parser, stream, state_builder);
	}

	return false;
}
