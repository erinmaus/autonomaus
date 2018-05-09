// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GUI_TAG_HPP
#define CHOMSKY_GUI_GUI_TAG_HPP

#include <set>
#include <string>

namespace chomsky
{
	class GUITags
	{
	public:
		GUITags() = default;
		~GUITags() = default;

		void add(const std::string& tag);
		void remove(const std::string& tag);

		bool has(const std::string& tag) const;

		bool match(const GUITags& other) const;

		std::size_t count() const;
		bool empty() const;

	private:
		typedef std::set<std::string> TagCollection;
		TagCollection tags;

	public:
		typedef TagCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
