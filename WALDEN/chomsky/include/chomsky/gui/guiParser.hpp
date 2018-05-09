// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GUI_PARSER_HPP
#define CHOMSKY_GUI_GUI_PARSER_HPP

#include <vector>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/parser/node.hpp"
#include "chomsky/resources/textureDictionary.hpp"
#include "chomsky/resources/fontExtentManager.hpp"
#include "kompvter/graphics/textureManager.hpp"

namespace chomsky
{
	class GUIContent;
	class GUIElementStream;

	class GUIParser
	{
	public:
		GUIParser(
			const kompvter::TextureManager& texture_manager,
			const TextureDictionary& dictionary);
		~GUIParser() = default;

		const kompvter::TextureManager& get_texture_manager() const;
		const TextureDictionary& get_texture_dictionary() const;

		FontExtentManager& get_font_extent_manager();
		const FontExtentManager& get_font_extent_manager() const;

		bool parse(GUIElementStream& stream, GUIContent& result);

		void add_node(parser::Node* node);
		void set_root_node(parser::Node* node);
		void clear_nodes();

	private:
		const kompvter::TextureManager* texture_manager;
		const TextureDictionary* texture_dictionary;
		FontExtentManager font_extent_manager;

		typedef std::unique_ptr<parser::Node> NodePointer;
		std::vector<NodePointer> nodes;
		parser::Node* root_node;
	};
}

#endif
