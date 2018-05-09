// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_PRINT_STATEMENT_NODE_HPP
#define CHOMSKY_GUI_GPD_PRINT_STATEMENT_NODE_HPP

#include "chomsky/gui/gpd/node.hpp"

namespace chomsky { namespace gpd
{
	class PrintStatementNode : public Node
	{
	public:
		PrintStatementNode() = default;
		~PrintStatementNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;
	};
} }

#endif
