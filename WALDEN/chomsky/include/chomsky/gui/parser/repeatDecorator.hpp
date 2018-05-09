// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_REPEAT_DECORATOR_HPP
#define CHOMSKY_GUI_PARSER_REPEAT_DECORATOR_HPP

#include "chomsky/gui/parser/node.hpp"

namespace chomsky { namespace parser
{
	class RepeatDecorator : public Node
	{
	public:
		RepeatDecorator() = default;
		~RepeatDecorator() = default;

		void set_iterations(int value);
		void set_min_iterations(int value);
		void set_max_iterations(int value);

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;

	private:
		int min_iterations = 1;
		int max_iterations = 0;
	};
} }

#endif
