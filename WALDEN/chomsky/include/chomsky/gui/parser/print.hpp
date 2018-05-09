// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_PRINT_NODE_HPP
#define CHOMSKY_GUI_PARSER_PRINT_NODE_HPP

#include <set>
#include <string>
#include "chomsky/gui/parser/node.hpp"

namespace chomsky { namespace parser
{
	class Print : public Node
	{
	public:
		Print() = default;
		~Print() = default;

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;
	};
} }

#endif
