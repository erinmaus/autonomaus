// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_FIND_DECORATOR_HPP
#define CHOMSKY_GUI_PARSER_FIND_DECORATOR_HPP

#include "chomsky/gui/parser/node.hpp"

namespace chomsky { namespace parser
{
	class FindDecorator : public Node
	{
	public:
		FindDecorator() = default;
		~FindDecorator() = default;

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;
	};
} }

#endif
