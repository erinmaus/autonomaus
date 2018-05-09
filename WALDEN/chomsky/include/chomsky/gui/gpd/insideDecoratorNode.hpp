// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_INSIDE_DECORATOR_NODE_HPP
#define CHOMSKY_GUI_GPD_INSIDE_DECORATOR_NODE_HPP

#include "chomsky/gui/parser/insideDecorator.hpp"
#include "chomsky/gui/gpd/parameterlessDecoratorNode.hpp"

namespace chomsky { namespace gpd
{
	class InsideDecoratorNode :
		public ParameterlessDecoratorNode<parser::InsideDecorator>
	{
	public:
		InsideDecoratorNode() = default;
		~InsideDecoratorNode() = default;

	protected:
		std::string get_keyword() const override;
	};
} }

inline std::string chomsky::gpd::InsideDecoratorNode::get_keyword() const
{
	return "inside";
}

#endif
