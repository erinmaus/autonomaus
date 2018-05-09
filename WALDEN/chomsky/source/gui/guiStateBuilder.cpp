// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"

chomsky::GUIStateBuilder::GUIStateBuilder(
	const TextureDictionary& texture_dictionary)
{
	this->texture_dictionary = &texture_dictionary;
	this->content_stack.emplace_back();
}

const chomsky::GUIContent& chomsky::GUIStateBuilder::get_parent() const
{
	for (auto i = this->content_stack.rbegin(); i != this->content_stack.rend(); ++i)
	{
		auto& content = i->content;
		if (content->is_widget())
		{
			return *content;
		}
	}

	return *this->content_stack.front().content;
}

const chomsky::GUIContent& chomsky::GUIStateBuilder::get_current() const
{
	return *this->content_stack.back().content;
}

void chomsky::GUIStateBuilder::enter(bool copy)
{
	if (copy)
	{
		this->content_stack.emplace_back(this->content_stack.back());
	}
	else
	{
		this->content_stack.emplace_back(GUIContent());
	}
}

void chomsky::GUIStateBuilder::enter_field(const std::string& name)
{
	auto& top = this->content_stack.back().content;
	if (top->has(name))
	{
		this->content_stack.emplace_back(top->get(name));
	}
	else
	{
		this->content_stack.emplace_back(name);
	}
}

void chomsky::GUIStateBuilder::enter_array()
{
	assert(!this->content_stack.empty());
	auto& top = this->content_stack.back().content;
	this->content_stack.emplace_back(top->get_name(), true);
}

void chomsky::GUIStateBuilder::enter_array(std::size_t index)
{
	assert(!this->content_stack.empty());
	auto& top = this->content_stack.back().content;
	this->content_stack.emplace_back(top->get(top->get_name(), index), true);
}

void chomsky::GUIStateBuilder::discard()
{
	this->content_stack.pop_back();
	assert(!this->content_stack.empty());
}

void chomsky::GUIStateBuilder::replace()
{
	assert(!this->content_stack.empty());
	auto top = this->content_stack.back();
	this->content_stack.pop_back();
	
	assert(!this->content_stack.empty());
	this->content_stack.pop_back();

	this->content_stack.push_back(top);
}

void chomsky::GUIStateBuilder::leave()
{
	assert(!this->content_stack.empty());
	auto top = this->content_stack.back();
	this->content_stack.pop_back();

	assert(!this->content_stack.empty());
	auto& current = this->content_stack.back().content;
	if (!top.is_array && current->has(top.content->get_name()))
	{
		current.writable()->set(top.content->get_name(), *top.content);
	}
	else
	{
		current.writable()->add(top.content->get_name(), *top.content);
	}
}

void chomsky::GUIStateBuilder::leave(std::size_t index)
{
	assert(!this->content_stack.empty());
	auto top = this->content_stack.back();
	assert(top.is_array);
	this->content_stack.pop_back();

	assert(!this->content_stack.empty());
	auto& current = this->content_stack.back().content;
	current.writable()->set(top.content->get_name(), index, *top.content);
}

void chomsky::GUIStateBuilder::make_widget(const std::string& name)
{
	assert(!this->content_stack.empty());
	auto& top = this->content_stack.back().content;
	top.writable()->set_widget_class(name);
}

void chomsky::GUIStateBuilder::collect(const GUIElement& element)
{
	assert(!this->content_stack.empty());
	auto& current = this->content_stack.back().content;

	current.writable()->add_element(element);

	if (element.get_is_textured() &&
		this->texture_dictionary->has(element.get_key()))
	{
		auto& entry = this->texture_dictionary->get(
			element.get_key());

		auto writable = current.writable();
		auto& tags = writable->get_tags();
		for (auto i = entry.tags_begin(); i != entry.tags_end(); ++i)
		{
			tags.add(*i);
		}
	}
}

void chomsky::GUIStateBuilder::add_tag(const std::string& tag)
{
	assert(!this->content_stack.empty());
	auto& current = this->content_stack.back().content;

	current.writable()->get_tags().add(tag);
}

void chomsky::GUIStateBuilder::add_quad(int x, int y, int width, int height)
{
	assert(!this->content_stack.empty());
	auto& current = this->content_stack.back().content;

	GUIElement::Vertex vertices[GUIElement::NUM_VERTICES];
	vertices[GUIElement::TOP_LEFT].position = glm::ivec2(x, y + height);
	vertices[GUIElement::TOP_RIGHT].position = glm::ivec2(x + width, y + height);
	vertices[GUIElement::BOTTOM_LEFT].position = glm::ivec2(x, y);
	vertices[GUIElement::BOTTOM_RIGHT].position = glm::ivec2(x + width, y);

	GUIElement element(vertices, GUIElement::NUM_VERTICES);
	current.writable()->add_element(element);
}

void chomsky::GUIStateBuilder::add_text(
	const std::string& text, const glm::vec4& color)
{
	assert(!this->content_stack.empty());
	auto& current = this->content_stack.back().content;
	current.writable()->get_text().add(text, color);
}

const chomsky::GUIContent& chomsky::GUIStateBuilder::get_root() const
{
	assert(this->content_stack.size() == 1);
	return *this->content_stack.back().content;
}

const chomsky::GUIElement& chomsky::GUIStateBuilder::get_failed() const
{
	return this->failed_element;
}

void chomsky::GUIStateBuilder::set_failed(const GUIElement& value)
{
	this->failed_element = value;
}

chomsky::GUIStateBuilder::StackEntry::StackEntry()
{
	// Nothing.
}

chomsky::GUIStateBuilder::StackEntry::StackEntry(
	const GUIContent& content, bool is_array) :
		content(content), is_array(is_array)
{
	// Nothing.
}

chomsky::GUIStateBuilder::StackEntry::StackEntry(
	const std::string& name, bool is_array) :
		content(GUIContent(name)), is_array(is_array)
{
	// Nothing.
}
