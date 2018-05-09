// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_SKIP_DECORATOR_NODE_HPP
#define CHOMSKY_GUI_GPD_SKIP_DECORATOR_NODE_HPP

#include "chomsky/gui/parser/skipDecorator.hpp"
#include "chomsky/gui/gpd/parameterlessDecoratorNode.hpp"

namespace chomsky { namespace gpd
{
	class SkipDecoratorNode :
		public ParameterlessDecoratorNode<parser::SkipDecorator>
	{
	public:
		SkipDecoratorNode() = default;
		~SkipDecoratorNode() = default;

	protected:
		std::string get_keyword() const override;
	};
} }

inline std::string chomsky::gpd::SkipDecoratorNode::get_keyword() const
{
	return "skip";
}

#endif
