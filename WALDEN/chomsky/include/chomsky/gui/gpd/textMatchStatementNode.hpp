// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_TEXT_MATCH_STATEMENT_NODE_HPP
#define CHOMSKY_GUI_GPD_TEXT_MATCH_STATEMENT_NODE_HPP

#include <string>
#include "chomsky/gui/gpd/node.hpp"

namespace chomsky { namespace gpd
{
	class ParameterListNode;
	class TagCollectionNode;

	class TextMatchStatementNode : public Node
	{
	public:
		TextMatchStatementNode() = default;
		~TextMatchStatementNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

	private:
		std::string font;
		ParameterListNode* parameters;
		TagCollectionNode* tags;
	};
} }

#endif
