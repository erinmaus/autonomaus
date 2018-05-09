// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_GROUP_NODE_HPP
#define CHOMSKY_GUI_PARSER_GROUP_NODE_HPP

#include <vector>
#include "chomsky/gui/parser/node.hpp"

namespace chomsky { namespace parser
{
	class Group : public Node
	{
	public:
		Group() = default;
		~Group() = default;

		enum
		{
			qualifier_all = 0,
			qualifier_any
		};
		void set_qualifier(int value);

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;

	private:
		int qualifier = qualifier_all;
	};
} }

#endif
