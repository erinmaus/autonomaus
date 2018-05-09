// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_OPTIONAL_DECORATOR_NODE_HPP
#define CHOMSKY_GUI_GPD_OPTIONAL_DECORATOR_NODE_HPP

#include "chomsky/gui/parser/optionalDecorator.hpp"
#include "chomsky/gui/gpd/parameterlessDecoratorNode.hpp"

namespace chomsky { namespace gpd
{
	class OptionalDecoratorNode :
		public ParameterlessDecoratorNode<parser::OptionalDecorator>
	{
	public:
		OptionalDecoratorNode() = default;
		~OptionalDecoratorNode() = default;

	protected:
		std::string get_keyword() const override;
	};
} }

inline std::string chomsky::gpd::OptionalDecoratorNode::get_keyword() const
{
	return "optional";
}

#endif
