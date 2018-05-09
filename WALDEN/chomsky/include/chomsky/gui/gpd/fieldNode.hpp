// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_FIELD_NODE_HPP
#define CHOMSKY_GUI_GPD_FIELD_NODE_HPP

#include <string>
#include "chomsky/gui/gpd/node.hpp"

namespace chomsky { namespace gpd
{
	class FieldNode : public Node
	{
	public:
		FieldNode() = default;
		~FieldNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

	private:
		Node* child;
		std::string name;
		bool is_array;
	};
} }

#endif
