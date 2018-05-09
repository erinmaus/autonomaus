// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/repeatDecorator.hpp"

void chomsky::parser::RepeatDecorator::set_iterations(int value)
{
	this->min_iterations = value;
	this->max_iterations = value;
}

void chomsky::parser::RepeatDecorator::set_min_iterations(int value)
{
	this->min_iterations = value;
}

void chomsky::parser::RepeatDecorator::set_max_iterations(int value)
{
	this->max_iterations = value;
}

bool chomsky::parser::RepeatDecorator::process(
	GUIParser& parser,
	GUIElementStream& stream,
	GUIStateBuilder& state_builder) const
{
	stream.push();

	int matches = 0;
	do
	{
		bool success;
		if (back() == nullptr)
		{
			success = false;
		}
		else
		{
			success = back()->process(parser, stream, state_builder);
		}

		if (!success)
		{
			if (matches < this->min_iterations)
			{
				stream.pop(false);
				return false;
			}
			else
			{
				break;
			}
		}
		else
		{
			++matches;
		}

		if (this->max_iterations != 0 && matches >= this->max_iterations)
		{
			assert(matches == this->max_iterations);
			break;
		}
	} while (true);

	stream.pop(true);
	return true;
}
