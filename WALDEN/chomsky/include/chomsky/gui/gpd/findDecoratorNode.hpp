// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_FIND_DECORATOR_NODE_HPP
#define CHOMSKY_GUI_GPD_FIND_DECORATOR_NODE_HPP

#include "chomsky/gui/parser/findDecorator.hpp"
#include "chomsky/gui/gpd/parameterlessDecoratorNode.hpp"

namespace chomsky { namespace gpd
{
	class FindDecoratorNode :
		public ParameterlessDecoratorNode<parser::FindDecorator>
	{
	public:
		FindDecoratorNode() = default;
		~FindDecoratorNode() = default;

	protected:
		std::string get_keyword() const override;
	};
} }

inline std::string chomsky::gpd::FindDecoratorNode::get_keyword() const
{
	return "find";
}

#endif
