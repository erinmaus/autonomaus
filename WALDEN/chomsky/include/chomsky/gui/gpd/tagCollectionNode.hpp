// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_TAG_COLLECTION_NODE_HPP
#define CHOMSKY_GUI_GPD_TAG_COLLECTION_NODE_HPP

#include <set>
#include <string>
#include "chomsky/gui/gpd/node.hpp"

namespace chomsky { namespace gpd
{
	class TagCollectionNode : public Node
	{
	public:
		TagCollectionNode() = default;
		~TagCollectionNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

	private:
		typedef std::set<std::string> TagCollection;
		TagCollection tags;

	public:
		typedef TagCollection::const_iterator const_tag_iterator;

		const_tag_iterator tags_begin() const;
		const_tag_iterator tags_end() const;
	};
} }

#endif
