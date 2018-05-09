// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_MATCH_STATEMENT_NODE_HPP
#define CHOMSKY_GUI_GPD_MATCH_STATEMENT_NODE_HPP

#include "chomsky/gui/gpd/node.hpp"

namespace chomsky { namespace gpd
{
	class MatchStatementNode : public Node
	{
	public:
		MatchStatementNode() = default;
		~MatchStatementNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

		static bool match(const std::string& identifier);

	private:
		Node* child;
	};
} }

#endif
