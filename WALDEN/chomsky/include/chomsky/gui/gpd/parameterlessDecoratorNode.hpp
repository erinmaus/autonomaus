// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_PARAMETERLESS_DECORATOR_NODE_HPP
#define CHOMSKY_GUI_GPD_PARAMETERLESS_DECORATOR_NODE_HPP

#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/expressionNode.hpp"
#include "chomsky/gui/gpd/node.hpp"
#include "chomsky/gui/gpd/tokenizer.hpp"

namespace chomsky { namespace gpd
{
	template <typename ParserNode>
	class ParameterlessDecoratorNode : public Node
	{
	public:
		ParameterlessDecoratorNode() = default;
		~ParameterlessDecoratorNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

	protected:
		virtual std::string get_keyword() const = 0;

	private:
		Node* child;
	};
} }

template <typename ParserNode>
void chomsky::gpd::ParameterlessDecoratorNode<ParserNode>::parse(Tokenizer& tokenizer)
{
	tokenizer.match(Token::type_identifier, get_keyword());
	this->child = add_child<ExpressionNode>();
	this->child->parse(tokenizer);
}

template <typename ParserNode>
void chomsky::gpd::ParameterlessDecoratorNode<ParserNode>::resolve(Compiler& compiler) const
{
	this->child->resolve(compiler);
}

template <typename ParserNode>
void chomsky::gpd::ParameterlessDecoratorNode<ParserNode>::compile(Compiler& compiler) const
{
	auto decorator = new ParserNode();

	compiler.add_node(decorator);
	compiler.top_node()->add(decorator);
	compiler.push_node(decorator);

	this->child->compile(compiler);

	compiler.pop_node();
}

#endif
