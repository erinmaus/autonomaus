// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_PARSER_MATCH_TEXT_NODE_HPP
#define CHOMSKY_GUI_PARSER_MATCH_TEXT_NODE_HPP

#include <set>
#include <string>
#include <glm/glm.hpp>
#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/parser/node.hpp"
#include "kompvter/graphics/texture.hpp"
#include "kompvter/graphics/textureRegion.hpp"

namespace chomsky { namespace parser
{
	class MatchText : public Node
	{
	public:
		MatchText() = default;
		~MatchText() = default;

		void set_font(const std::string& value);
		void set_wildcard();
		void add_font_tag(const std::string& tag);
		void set_shadowed(bool value);
		void set_include_spaces(bool value);
		void set_break_on_newline(bool value);
		void set_break_on_color(bool value);

		bool process(
			GUIParser& parser,
			GUIElementStream& stream,
			GUIStateBuilder& state_builder) const override;

	private:
		struct CharacterResult
		{
			std::string character;
			GUIElement element;
			kompvter::Texture texture;
			glm::ivec2 offset;
			glm::ivec2 size;
			int space_width;
			const kompvter::TextureRegion* texture_region;
			bool skip;
			bool strike_through;
		};
		bool try_process_character(
			GUIParser& parser,
			GUIElementStream& stream,
			CharacterResult& previous,
			CharacterResult& result) const;
		bool skip_strike_through(
			GUIElementStream& stream,
			CharacterResult& previos) const;
		bool try_get_character(
			GUIParser& parser,
			GUIElementStream& stream,
			CharacterResult& result) const;

		static bool is_space(const std::string& c);

		std::string font = "*";
		std::set<std::string> tags;
		bool shadowed = false;
		bool include_spaces = true;
		bool break_on_newline = true;
		bool break_on_color = false;
	};
} }

#endif
