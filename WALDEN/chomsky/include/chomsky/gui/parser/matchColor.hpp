// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_MATCH_COLOR_NODE_HPP
#define CHOMSKY_GUI_PARSER_MATCH_COLOR_NODE_HPP

#include <glm/glm.hpp>
#include "chomsky/gui/parser/node.hpp"

namespace chomsky { namespace parser
{
	class MatchColor : public Node
	{
	public:
		MatchColor() = default;
		~MatchColor() = default;

		void set_color(const glm::ivec4& value);

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;

	private:
		glm::ivec4 color = glm::ivec4(255);
	};
} }

#endif
