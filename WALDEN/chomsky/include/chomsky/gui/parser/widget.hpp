// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_WIDGET_NODE_HPP
#define CHOMSKY_GUI_PARSER_WIDGET_NODE_HPP

#include <set>
#include <string>
#include "chomsky/gui/parser/node.hpp"

namespace chomsky { namespace parser
{
	class Widget : public Node
	{
	public:
		Widget() = default;
		~Widget() = default;

		void set_name(const std::string& name);
		void add_tag(const std::string& tag);

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;

	private:
		std::string name;
		std::set<std::string> tags;
	};
} }

#endif
