// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_EXPRESSION_NODE_HPP
#define CHOMSKY_GUI_GPD_EXPRESSION_NODE_HPP

#include "chomsky/gui/gpd/node.hpp"

namespace chomsky { namespace gpd
{
	class DecoratorNode;
	class StatementNode;

	class ExpressionNode : public Node
	{
	public:
		ExpressionNode() = default;
		~ExpressionNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

	private:
		Node* child;
	};
} }

#endif
