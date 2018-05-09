// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_PEEK_DECORATOR_NODE_HPP
#define CHOMSKY_GUI_GPD_PEEK_DECORATOR_NODE_HPP

#include "chomsky/gui/parser/peekDecorator.hpp"
#include "chomsky/gui/gpd/parameterlessDecoratorNode.hpp"

namespace chomsky { namespace gpd
{
	class PeekDecoratorNode :
		public ParameterlessDecoratorNode<parser::PeekDecorator>
	{
	public:
		PeekDecoratorNode() = default;
		~PeekDecoratorNode() = default;

	protected:
		std::string get_keyword() const override;
	};
} }

inline std::string chomsky::gpd::PeekDecoratorNode::get_keyword() const
{
	return "peek";
}

#endif
