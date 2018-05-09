// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_WIDGET_NODE_HPP
#define CHOMSKY_GUI_GPD_WIDGET_NODE_HPP

#include <set>
#include <string>
#include "chomsky/gui/gpd/node.hpp"
#include "chomsky/gui/parser/widget.hpp"

namespace chomsky { namespace gpd
{
	class ExpressionNode;
	class TagCollectionNode;

	class WidgetNode : public Node
	{
	public:
		WidgetNode() = default;
		~WidgetNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

	private:
		ExpressionNode* expression;
		std::string name;
		TagCollectionNode* tags;

		mutable parser::Widget* widget;
	};
} }

#endif
