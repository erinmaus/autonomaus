// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_NODE_HPP
#define CHOMSKY_GUI_PARSER_NODE_HPP

#include <vector>

namespace chomsky
{
	class GUIElementStream;
	class GUIParser;
	class GUIStateBuilder;
};

namespace chomsky { namespace parser
{
	class Node
	{
	public:
		virtual ~Node() = default;

		void add(const Node* node);

		virtual bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const = 0;

	protected:
		const Node* back() const;

		typedef std::vector<const Node*>::const_iterator const_iterator;
		const_iterator begin() const;
		const_iterator end() const;

	private:
		std::vector<const Node*> children;
	};
} }

#endif
