// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiContent.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"

chomsky::GUIParser::GUIParser(
	const kompvter::TextureManager& texture_manager,
	const TextureDictionary& dictionary)
{
	this->texture_manager = &texture_manager;
	this->texture_dictionary = &dictionary;
}

const kompvter::TextureManager& chomsky::GUIParser::get_texture_manager() const
{
	return *this->texture_manager;
}

const chomsky::TextureDictionary& chomsky::GUIParser::get_texture_dictionary() const
{
	return *this->texture_dictionary;
}

chomsky::FontExtentManager& chomsky::GUIParser::get_font_extent_manager()
{
	return this->font_extent_manager;
}

const chomsky::FontExtentManager& chomsky::GUIParser::get_font_extent_manager() const
{
	return this->font_extent_manager;
}

bool chomsky::GUIParser::parse(GUIElementStream& stream, GUIContent& result)
{
	if (this->root_node == nullptr)
	{
		return false;
	}

	GUIStateBuilder state_builder(*this->texture_dictionary);
	if (!this->root_node->process(*this, stream, state_builder))
	{
		stream.seek(state_builder.get_failed().get_offset());
		return false;
	}

	result = state_builder.get_root();
	return true;
}

void chomsky::GUIParser::add_node(parser::Node* node)
{
	this->nodes.emplace_back(node);
}

void chomsky::GUIParser::set_root_node(parser::Node* node)
{
	this->root_node = node;
}

void chomsky::GUIParser::clear_nodes()
{
	this->root_node = nullptr;
	this->nodes.clear();
}
