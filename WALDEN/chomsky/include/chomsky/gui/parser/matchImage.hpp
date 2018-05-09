// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_MATCH_IMAGE_NODE_HPP
#define CHOMSKY_GUI_PARSER_MATCH_IMAGE_NODE_HPP

#include <set>
#include <string>
#include "chomsky/gui/parser/node.hpp"

namespace chomsky { namespace parser
{
	class MatchImage : public Node
	{
	public:
		MatchImage() = default;
		~MatchImage() = default;

		void set_query(const std::string& value);
		void set_wildcard();
		void add_query_tag(const std::string& value);

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;

	private:
		std::string query = "*";
		std::set<std::string> tags;
	};
} }

#endif
