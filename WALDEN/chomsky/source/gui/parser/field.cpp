// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/field.hpp"

void chomsky::parser::Field::set_name(const std::string& value)
{
	this->name = value;
}

void chomsky::parser::Field::set_array(bool value)
{
	this->is_array = value;
}

bool chomsky::parser::Field::process(
	GUIParser& parser,
	GUIElementStream& stream,
	GUIStateBuilder& state_builder) const
{
	stream.push();

	if (back() == nullptr)
	{
		return false;
	}

	state_builder.enter_field(this->name);
	if (this->is_array)
	{
		bool success = false;
		auto& current = state_builder.get_current();
		std::size_t current_index = 0;
		do
		{
			if (current_index >= current.count(this->name))
			{
				state_builder.enter_array();
			}
			else
			{
				state_builder.enter_array(current_index);
			}

			if (!back()->process(parser, stream, state_builder))
			{
				state_builder.discard();
				break;
			}

			if (current_index >= current.count(this->name))
			{
				state_builder.leave();
			}
			else
			{
				state_builder.leave(current_index);
			}
			++current_index;

			success = true;
		} while(true);

		if (!success)
		{
			state_builder.discard();
			stream.pop(false);
			return false;
		}
	}
	else if (!back()->process(parser, stream, state_builder))
	{
		state_builder.discard();
		stream.pop(false);
		return false;
	}
	state_builder.leave();

	stream.pop(true);
	return true;
}
