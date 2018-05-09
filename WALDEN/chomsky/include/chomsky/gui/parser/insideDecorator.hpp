// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_INSIDE_DECORATOR_HPP
#define CHOMSKY_GUI_PARSER_INSIDE_DECORATOR_HPP

#include "chomsky/gui/parser/node.hpp"

namespace chomsky { namespace parser
{
	class InsideDecorator : public Node
	{
	public:
		InsideDecorator() = default;
		~InsideDecorator() = default;

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;
	};
} }

#endif
