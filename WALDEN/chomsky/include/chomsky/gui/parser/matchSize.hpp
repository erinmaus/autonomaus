// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_MATCH_SIZE_NODE_HPP
#define CHOMSKY_GUI_PARSER_MATCH_SIZE_NODE_HPP

#include "chomsky/gui/parser/node.hpp"

namespace chomsky { namespace parser
{
	class MatchSize : public Node
	{
	public:
		MatchSize() = default;
		~MatchSize() = default;

		void set_width(int value);
		void unset_width();
		void set_height(int value);
		void unset_height();

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;

	private:
		int width = 0;
		int height = 0;
	};
} }

#endif
