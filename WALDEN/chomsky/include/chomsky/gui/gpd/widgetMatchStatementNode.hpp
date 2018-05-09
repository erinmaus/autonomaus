// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_WIDGET_MATCH_STATEMENT_NODE_HPP
#define CHOMSKY_GUI_GPD_WIDGET_MATCH_STATEMENT_NODE_HPP

#include <string>
#include "chomsky/gui/gpd/node.hpp"

namespace chomsky { namespace gpd
{
	class TagCollectionNode;

	class WidgetMatchStatementNode : public Node
	{
	public:
		WidgetMatchStatementNode() = default;
		~WidgetMatchStatementNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

	private:
		std::string widget;
		TagCollectionNode* tags;
	};
} }

#endif
