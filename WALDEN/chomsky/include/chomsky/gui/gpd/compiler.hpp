// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_COMPILER_HPP
#define CHOMSKY_GUI_GPD_COMPILER_HPP

#include <vector>
#include <stack>
#include <string>
#include "chomsky/gui/gpd/tokenizer.hpp"
#include "chomsky/gui/parser/node.hpp"
#include "chomsky/gui/parser/widget.hpp"

namespace chomsky
{
	class GUIParser;
}

namespace chomsky { namespace gpd 
{
	class Compiler
	{
	public:
		Compiler() = default;
		~Compiler() = default;

		void add_input(const std::string& filename, const std::string& source);
		void compile(GUIParser& parser);

		void add_node(parser::Node* node);
		void push_node(parser::Node* node);
		void pop_node();
		parser::Node* top_node();

		void register_widget(
			parser::Widget* node,
			const std::string& name,
			const std::set<std::string>& tags);
		void for_each_widget(
			const std::string& name,
			const std::set<std::string>& tags,
			const std::function<void(parser::Widget*)>& iterator);

	private:
		struct Widget
		{
			std::string name;
			std::set<std::string> tags;
			parser::Widget* node;
		};
		std::vector<Widget> widgets;

		std::vector<Tokenizer> inputs;
		GUIParser* parser = nullptr;

		std::stack<parser::Node*> node_stack;
	};
} }

#endif
