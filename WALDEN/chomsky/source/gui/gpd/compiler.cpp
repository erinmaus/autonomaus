// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/gpd/rootNode.hpp"
#include "chomsky/gui/gpd/parsingError.hpp"

void chomsky::gpd::Compiler::add_input(
	const std::string& filename, const std::string& source)
{
	this->inputs.emplace_back(filename, source);
}

void chomsky::gpd::Compiler::compile(chomsky::GUIParser& parser)
{
	assert(this->parser == nullptr);
	this->parser = &parser;

	this->widgets.clear();
	parser.clear_nodes();

	std::vector<RootNode> root_nodes;
	for (auto& input: this->inputs)
	{
		RootNode node;
		node.parse(input);
		root_nodes.push_back(node);
	}

	for (auto& node: root_nodes)
	{
		node.resolve(*this);
	}
	assert(this->node_stack.empty());

	for (auto& node: root_nodes)
	{
		node.compile(*this);
	}
	assert(this->node_stack.empty());

	for (auto& widget: this->widgets)
	{
		if (widget.name == "root" && widget.tags.empty())
		{
			parser.set_root_node(widget.node);
			break;
		}
	}

	this->parser = nullptr;
}

void chomsky::gpd::Compiler::add_node(parser::Node* node)
{
	assert(this->parser != nullptr);
	this->parser->add_node(node);
}

void chomsky::gpd::Compiler::push_node(parser::Node* node)
{
	this->node_stack.push(node);
}

void chomsky::gpd::Compiler::pop_node()
{
	assert(!this->node_stack.empty());
	this->node_stack.pop();
}

chomsky::parser::Node* chomsky::gpd::Compiler::top_node()
{
	assert(!this->node_stack.empty());
	return this->node_stack.top();
}

void chomsky::gpd::Compiler::register_widget(
	parser::Widget* node,
	const std::string& name,
	const std::set<std::string>& tags)
{
	this->widgets.emplace_back();

	auto& top = this->widgets.back();
	top.node = node;
	top.name = name;
	top.tags = tags;
}

void chomsky::gpd::Compiler::for_each_widget(
	const std::string& name,
	const std::set<std::string>& tags,
	const std::function<void(parser::Widget*)>& iterator)
{
	for (auto& widget: this->widgets)
	{
		if (widget.name != name && !name.empty())
		{
			continue;
		}

		bool is_match = true;
		for (auto tag: tags)
		{
			if (widget.tags.count(tag) == 0)
			{
				is_match = false;
				break;
			}
		}

		if (!is_match)
		{
			continue;
		}

		iterator(widget.node);
	}
}
