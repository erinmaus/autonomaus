// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_REPEAT_DECORATOR_NODE_HPP
#define CHOMSKY_GUI_GPD_REPEAT_DECORATOR_NODE_HPP

#include "chomsky/gui/gpd/node.hpp"

namespace chomsky { namespace gpd
{
	class ParameterListNode;

	class RepeatDecoratorNode : public Node
	{
	public:
		RepeatDecoratorNode() = default;
		~RepeatDecoratorNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

	private:
		ParameterListNode* parameters = nullptr;
		Node* child;
	};
} }

#endif
