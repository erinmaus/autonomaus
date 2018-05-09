// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_PEEK_DECORATOR_HPP
#define CHOMSKY_GUI_PARSER_PEEK_DECORATOR_HPP

#include "chomsky/gui/parser/node.hpp"

namespace chomsky { namespace parser
{
	class PeekDecorator : public Node
	{
	public:
		PeekDecorator() = default;
		~PeekDecorator() = default;

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;
	};
} }

#endif
