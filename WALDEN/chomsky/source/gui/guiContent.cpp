// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiContent.hpp"

chomsky::GUIContent::GUIContent(const std::string& name)
{
	this->name = name;
}

const std::string& chomsky::GUIContent::get_name() const
{
	return this->name;
}

const std::string& chomsky::GUIContent::get_widget_class() const
{
	return this->widget_class;
}

void chomsky::GUIContent::set_widget_class(const std::string& value)
{
	this->widget_class = value;
}

void chomsky::GUIContent::unset_widget_class()
{
	this->widget_class.clear();
}

bool chomsky::GUIContent::is_widget() const
{
	return !this->widget_class.empty();
}

int chomsky::GUIContent::get_x() const
{
	update_position_size();
	return this->x;
}

int chomsky::GUIContent::get_y() const
{
	update_position_size();
	return this->y;
}

glm::ivec2 chomsky::GUIContent::get_position() const
{
	update_position_size();
	return glm::ivec2(this->x, this->y);
}

int chomsky::GUIContent::get_width() const
{
	update_position_size();
	return this->width;
}

int chomsky::GUIContent::get_height() const
{
	update_position_size();
	return this->height;
}

glm::ivec2 chomsky::GUIContent::get_dimensions() const
{
	return glm::ivec2(this->width, this->height);
}

chomsky::GUIText& chomsky::GUIContent::get_text()
{
	return this->text;
}

const chomsky::GUIText& chomsky::GUIContent::get_text() const
{
	return this->text;
}

chomsky::GUITags& chomsky::GUIContent::get_tags()
{
	return this->tags;
}

const chomsky::GUITags& chomsky::GUIContent::get_tags() const
{
	return this->tags;
}

std::size_t chomsky::GUIContent::count() const
{
	return this->children.size();
}

std::size_t chomsky::GUIContent::count(const std::string& name) const
{
	return this->children.count(name);
}

bool chomsky::GUIContent::empty() const
{
	return this->children.empty();
}

void chomsky::GUIContent::add(const std::string& name, const GUIContent& child)
{
	GUIContent content = child;
	content.name = name;

	this->children.emplace(name, GUIContentPointer(content));
	this->is_dirty = true;
}

void chomsky::GUIContent::remove(const std::string& name)
{
	this->children.erase(name);
	this->is_dirty = true;
}

bool chomsky::GUIContent::has(const std::string& name) const
{
	return this->children.count(name) != 0;
}

const chomsky::GUIContent& chomsky::GUIContent::get(
	const std::string& name, std::size_t index) const
{
	auto begin = this->children.lower_bound(name);
	auto end = this->children.upper_bound(name);

	std::size_t i = 0;
	while (i < index && begin != end)
	{
		++begin;
		++i;
	}

	assert(begin != end);
	return *begin->second;
}

void chomsky::GUIContent::set(const std::string& name, const GUIContent& content)
{
	auto p = mget(name).writable();
	*p = content;
	p->name = name;
}

void chomsky::GUIContent::set(
	const std::string& name, std::size_t index,
	const GUIContent& content)
{
	auto p = mget(name, index).writable();
	*p = content;
	p->name = name;
}

chomsky::GUIContent chomsky::GUIContent::try_get(
	const std::string& name, std::size_t index) const
{
	auto begin = this->children.lower_bound(name);
	auto end = this->children.upper_bound(name);

	std::size_t i = 0;
	while (i < index && begin != end)
	{
		++begin;
		++i;
	}

	if (begin == end)
	{
		return GUIContent();
	}

	return *begin->second;
}

void chomsky::GUIContent::add_element(const GUIElement& element)
{
	this->elements.push_back(element);
	this->is_dirty = true;
}

void chomsky::GUIContent::remove_element(std::size_t index)
{
	assert(index < this->elements.size());
	this->elements.erase(this->elements.begin() + index);
	this->is_dirty = true;
}

std::size_t chomsky::GUIContent::get_num_elements() const
{
	return this->elements.size();
}

const chomsky::GUIElement& chomsky::GUIContent::get_element(
	std::size_t index) const
{
	return this->elements.at(index);
}

chomsky::GUIContent::const_iterator chomsky::GUIContent::begin() const
{
	return this->children.begin();
}

chomsky::GUIContent::const_iterator chomsky::GUIContent::end() const
{
	return this->children.end();
}

kommon::Cow<chomsky::GUIContent>& chomsky::GUIContent::mget(
	const std::string& name, std::size_t index)
{
	auto begin = this->children.lower_bound(name);
	auto end = this->children.upper_bound(name);

	std::size_t i = 0;
	while (i < index && begin != end)
	{
		++begin;
		++i;
	}

	assert(begin != end);
	return begin->second;
}

void chomsky::GUIContent::update_position_size() const
{
	if (this->is_dirty)
	{
		int left = std::numeric_limits<int>::max();
		int right = std::numeric_limits<int>::min();
		int top = std::numeric_limits<int>::min();
		int bottom = std::numeric_limits<int>::max();

		bool expanded = false;
		if (!this->elements.empty())
		{
			for (auto& element: this->elements)
			{
				for (std::size_t i = 0; i < element.get_num_vertices(); ++i)
				{
					auto vertex = element.get_vertex(i);
					left = glm::min(vertex.position.x, left);
					right = glm::max(vertex.position.x, right);
					top = glm::max(vertex.position.y, top);
					bottom = glm::min(vertex.position.y, bottom);
				}
			}

			expanded = true;
		}

		for (auto& child: this->children)
		{
			if (child.second->expand_bounds(left, right, top, bottom))
			{
				expanded = true;
			}
		}

		if (expanded)
		{
			assert(left <= right);
			assert(bottom <= top);

			this->x = left;
			this->y = bottom;
			this->width = right - left;
			this->height = top - bottom;
		}
		else
		{
			this->x = 0;
			this->y = 0;
			this->width = 0;
			this->height = 0;
		}

		this->is_dirty = false;
	}
}

bool chomsky::GUIContent::expand_bounds(
	int& left, int& right, int& top, int& bottom) const
{
	update_position_size();

	bool has_bounds = !this->elements.empty();
	for (auto& child: this->children)
	{
		if (child.second->expand_bounds(left, right, top, bottom))
		{
			has_bounds = true;
		}
	}

	if (has_bounds)
	{
		left = glm::min(left, this->x);
		right = glm::max(right, this->x + this->width);
		top = glm::max(top, this->y + this->height);
		bottom = glm::min(bottom, this->y);
	}

	return has_bounds;
}
