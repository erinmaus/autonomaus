// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GUI_TEXT_HPP
#define CHOMSKY_GUI_GUI_TEXT_HPP

#include <string>
#include <tuple>
#include <vector>
#include <glm/glm.hpp>

namespace chomsky
{
	class GUIText
	{
	public:
		static const std::size_t SEGMENT_STRING = 0;
		static const std::size_t SEGMENT_COLOR = 1;

		typedef std::tuple<std::string, glm::vec4> Segment;

		GUIText() = default;
		~GUIText() = default;

		void add(const std::string& text, const glm::vec4& color);
		void remove(std::size_t index);

		Segment get(std::size_t index) const;

		std::size_t count() const;
		bool empty() const;

		const std::string& to_string() const;
		operator const std::string&() const;

	private:
		typedef std::vector<Segment> SegmentCollection;
		SegmentCollection segments;

		mutable std::string string;
		mutable bool is_string_dirty = false;

	public:
		typedef SegmentCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
