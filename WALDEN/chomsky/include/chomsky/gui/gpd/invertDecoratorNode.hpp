// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_INVERT_DECORATOR_NODE_HPP
#define CHOMSKY_GUI_GPD_INVERT_DECORATOR_NODE_HPP

#include "chomsky/gui/parser/invertDecorator.hpp"
#include "chomsky/gui/gpd/parameterlessDecoratorNode.hpp"

namespace chomsky { namespace gpd
{
	class InvertDecoratorNode :
		public ParameterlessDecoratorNode<parser::InvertDecorator>
	{
	public:
		InvertDecoratorNode() = default;
		~InvertDecoratorNode() = default;

	protected:
		std::string get_keyword() const override;
	};
} }

inline std::string chomsky::gpd::InvertDecoratorNode::get_keyword() const
{
	return "invert";
}

#endif
