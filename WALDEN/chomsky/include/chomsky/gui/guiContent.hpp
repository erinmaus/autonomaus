// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GUI_CONTENT_HPP
#define CHOMSKY_GUI_GUI_CONTENT_HPP

#include <map>
#include <glm/glm.hpp>
#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiTags.hpp"
#include "chomsky/gui/guiText.hpp"
#include "kommon/utility/lightSharedPointer.hpp"
#include "kommon/utility/cow.hpp"

namespace chomsky
{
	class GUIContent
	{
	public:
		explicit GUIContent(const std::string& name = "");
		~GUIContent() = default;

		const std::string& get_name() const;

		const std::string& get_widget_class() const;
		void set_widget_class(const std::string& value);
		void unset_widget_class();
		bool is_widget() const;

		int get_x() const;
		int get_y() const;
		glm::ivec2 get_position() const;

		int get_width() const;
		int get_height() const;
		glm::ivec2 get_dimensions() const;

		GUIText& get_text();
		const GUIText& get_text() const;

		GUITags& get_tags();
		const GUITags& get_tags() const;

		std::size_t count() const;
		std::size_t count(const std::string& name) const;

		bool empty() const;

		void add(const std::string& name, const GUIContent& child);
		void remove(const std::string& name);

		bool has(const std::string& name) const;
		const GUIContent& get(
			const std::string& name, std::size_t index = 0) const;
		void set(const std::string& name, const GUIContent& content);
		void set(const std::string& name, std::size_t index, const GUIContent& content);

		GUIContent try_get(const std::string& name, std::size_t index = 0) const;

		void add_element(const GUIElement& element);
		void remove_element(std::size_t index);
		std::size_t get_num_elements() const;
		const GUIElement& get_element(std::size_t index) const;

	private:
		kommon::Cow<GUIContent>& mget(const std::string& name, std::size_t index = 0);

		std::string name;
		std::string widget_class;

		mutable bool is_dirty = false;
		mutable int x = 0, y = 0;
		mutable int width = 0, height = 0;
		void update_position_size() const;
		bool expand_bounds(int& left, int& right, int& top, int& bottom) const;

		GUIText text;
		GUITags tags;

		typedef kommon::Cow<GUIContent> GUIContentPointer;
		typedef std::multimap<std::string, GUIContentPointer> ChildrenCollection;
		ChildrenCollection children;

		typedef std::vector<GUIElement> ElementCollection;
		ElementCollection elements;

	public:
		typedef ChildrenCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
