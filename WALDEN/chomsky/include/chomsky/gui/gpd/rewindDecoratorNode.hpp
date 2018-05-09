// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_REWIND_DECORATOR_NODE_HPP
#define CHOMSKY_GUI_GPD_REWIND_DECORATOR_NODE_HPP

#include "chomsky/gui/parser/rewindDecorator.hpp"
#include "chomsky/gui/gpd/parameterlessDecoratorNode.hpp"

namespace chomsky { namespace gpd
{
	class RewindDecoratorNode :
		public ParameterlessDecoratorNode<parser::RewindDecorator>
	{
	public:
		RewindDecoratorNode() = default;
		~RewindDecoratorNode() = default;

	protected:
		std::string get_keyword() const override;
	};
} }

inline std::string chomsky::gpd::RewindDecoratorNode::get_keyword() const
{
	return "rewind";
}

#endif
