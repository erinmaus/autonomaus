// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_FIELD_NODE_HPP
#define CHOMSKY_GUI_PARSER_FIELD_NODE_HPP

#include <string>
#include "chomsky/gui/parser/node.hpp"

namespace chomsky { namespace parser 
{
	class Field : public Node
	{
	public:
		Field() = default;
		~Field() = default;

		void set_name(const std::string& value);
		void set_array(bool value);

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;

	private:
		std::string name;
		bool is_array = false;
	};
} }

#endif
