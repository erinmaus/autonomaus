// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_NODE_HPP
#define CHOMSKY_GUI_GPD_NODE_HPP

#include <memory>
#include <vector>

namespace chomsky { namespace gpd 
{
	class Compiler;
	class Tokenizer;
	class Walker;

	class Node
	{
	public:
		virtual ~Node() = default;

		virtual void parse(Tokenizer& tokenizer) = 0;
		virtual void resolve(Compiler& compiler) const = 0;
		virtual void compile(Compiler& compiler) const = 0;

	protected:
		template <typename Child>
		Child* add_child();

	private:
		typedef std::shared_ptr<Node> NodePointer;
		std::vector<NodePointer> children;
	};
} }

template <typename Child>
Child* chomsky::gpd::Node::add_child()
{
	this->children.emplace_back(new Child());
	return (Child*)this->children.back().get();
}

#endif
