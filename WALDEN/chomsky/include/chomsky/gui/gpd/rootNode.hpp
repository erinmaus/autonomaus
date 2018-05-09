// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_ROOT_NODE_HPP
#define CHOMSKY_GUI_GPD_ROOT_NODE_HPP

#include "chomsky/gui/gpd/node.hpp"

namespace chomsky { namespace gpd
{
	class WidgetNode;

	class RootNode : public Node
	{
	public:
		RootNode() = default;
		~RootNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

	private:
		std::vector<WidgetNode*> widgets;
	};
} }

#endif
