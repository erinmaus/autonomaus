// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_COLOR_MATCH_STATEMENT_NODE_HPP
#define CHOMSKY_GUI_GPD_COLOR_MATCH_STATEMENT_NODE_HPP

#include <glm/glm.hpp>
#include "chomsky/gui/gpd/node.hpp"

namespace chomsky { namespace gpd
{
	class ColorMatchStatementNode : public Node
	{
	public:
		ColorMatchStatementNode() = default;
		~ColorMatchStatementNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

	private:
		glm::ivec4 color;
	};
} }

#endif
