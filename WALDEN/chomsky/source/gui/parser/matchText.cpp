// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <string>
#include "chomsky/chomsky.hpp"
#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/matchText.hpp"

void chomsky::parser::MatchText::set_font(const std::string& value)
{
	this->font = value;
}

void chomsky::parser::MatchText::set_wildcard()
{
	this->font = "*";
}

void chomsky::parser::MatchText::add_font_tag(const std::string& tag)
{
	this->tags.insert(tag);
}

void chomsky::parser::MatchText::set_shadowed(bool value)
{
	this->shadowed = value;
}

void chomsky::parser::MatchText::set_include_spaces(bool value)
{
	this->include_spaces = value;
}

void chomsky::parser::MatchText::set_break_on_newline(bool value)
{
	this->break_on_newline = value;
}

void chomsky::parser::MatchText::set_break_on_color(bool value)
{
	this->break_on_color = value;
}

bool chomsky::parser::MatchText::process(
	GUIParser& parser,
	GUIElementStream& stream,
	GUIStateBuilder& state_builder) const
{
	stream.push();

	CharacterResult current_character = {};
	if (!try_process_character(parser, stream, current_character, current_character))
	{
		stream.pop(false);
		return false;
	}
	std::string current_text;
	current_text += current_character.character;
	state_builder.collect(current_character.element);

	auto previous_position = stream.tell();
	CharacterResult next_character;
	while (try_process_character(parser, stream, current_character, next_character))
	{
		if (!next_character.skip)
		{
			int offset_x = next_character.element.get_x();
			offset_x -= current_character.element.get_x();
			offset_x -= current_character.size.x;

			int offset_y = (next_character.element.get_y());
			offset_y -= (current_character.element.get_y());

			int cell_width = current_character.texture_region->get_width() / 16;
			int cell_height = current_character.texture_region->get_height() / 16;

			int space_width;
			if (next_character.space_width != 0)
			{
				space_width = next_character.space_width;
			}
			else
			{
				space_width = cell_width / 4;
			}

			bool is_literal_space = is_space(next_character.character) || is_space(current_character.character);
			bool is_gap_space = offset_x > space_width;
			bool is_gap_newline = std::abs(offset_y) >= cell_height * 0.75f && !is_literal_space;
			bool is_gap_space_break = (offset_x >= cell_width || (offset_x < 0 && !is_gap_newline)) && !is_literal_space;
			bool is_gap_newline_break = is_gap_newline && (offset_y > 0 || std::abs(offset_y) > cell_height * 2.0f || this->break_on_newline);

			if (this->include_spaces)
			{
				if (is_gap_newline)
				{
					current_text += "\n";
				}
				else if (is_gap_space)
				{
					current_text += " ";
				}
			}

			bool is_color_break = current_character.element.get_color() != next_character.element.get_color();
			if (is_gap_space_break || is_gap_newline_break || (is_color_break && this->break_on_color))
			{
				stream.seek(previous_position);
				break;
			}

			if (current_character.element.get_color() != next_character.element.get_color())
			{
				state_builder.add_text(current_text, current_character.element.get_color());
				current_text.clear();
			}

			if (!is_space(next_character.character))
			{
				current_text += next_character.character;
			}

			if (next_character.strike_through)
			{
				state_builder.add_tag("font-strike-through");
			}
		}

		state_builder.collect(next_character.element);
		current_character = next_character;
		previous_position = stream.tell();
	}

	if (current_character.strike_through)
	{
		skip_strike_through(stream, current_character);
	}

	state_builder.add_text(current_text, current_character.element.get_color());

	stream.pop(true);
	return true;
}

bool chomsky::parser::MatchText::try_process_character(
	GUIParser& parser, GUIElementStream& stream, CharacterResult& previous, CharacterResult& result) const
{
	if (previous.strike_through)
	{
		skip_strike_through(stream, previous);
	}

	if (!try_get_character(parser, stream, result))
	{
		return false;
	}

	bool success = true;
	if (this->shadowed && !result.skip)
	{
		success = try_get_character(parser, stream, result);
	}

	if (success)
	{
		stream.push();

		GUIElement element;
		if (stream.next(element))
		{
			if (!element.get_is_textured() && element.get_height() <= 2)
			{
				result.strike_through = true;
			}
			else
			{
				result.strike_through = false;
			}
		}
		
		stream.pop(false);
	}

	return success;
}

bool chomsky::parser::MatchText::skip_strike_through(
	GUIElementStream& stream,
	CharacterResult& previous) const
{
	stream.push();
	
	GUIElement element;
	while (stream.next(element))
	{
		if (element.get_is_textured() || element.get_height() > 2)
		{
			break;
		}

		stream.pop(true);
		stream.push();
	}

	stream.pop(false);
	return true;
}

bool chomsky::parser::MatchText::try_get_character(
	GUIParser& parser, GUIElementStream& stream, CharacterResult& result) const
{
	stream.push();
	if (!stream.next(result.element))
	{
		stream.pop(false);
		return false;
	}

	if (!result.element.get_is_textured())
	{
		stream.pop(false);
		return false;
	}

	auto& dictionary = parser.get_texture_dictionary();
	if (!dictionary.has(result.element.get_key()))
	{
		stream.pop(false);
		return false;
	}

	auto& entry = dictionary.get(result.element.get_key());
	if (!entry.has_tag("font"))
	{
		stream.pop(false);
		return false;
	}

	if (entry.has_tag("x-font-skip"))
	{
		result.skip = true;

		stream.pop(true);
		return true;
	}
	else
	{
		result.skip = false;
	}

	if (this->font != "*" && entry.get_value() != this->font)
	{
		stream.pop(false);
		return false;
	}

	for (auto tag: this->tags)
	{
		if (!entry.has_tag(tag))
		{
			stream.pop(false);
			return false;
		}
	}

	if (entry.has_data("font-space-width"))
	{
		auto value = entry.get_data("font-space-width");
		result.space_width = std::stoi(value);
	}
	else
	{
		result.space_width = 0;
	}

	auto& texture_manager = parser.get_texture_manager();
	if (!texture_manager.has_texture(result.element.get_texture_name()))
	{
		stream.pop(false);
		return false;
	}

	result.texture = texture_manager.get_texture(result.element.get_texture_name());
	result.texture_region = result.texture.get_region_from_name(result.element.get_texture_region_name());
	if (result.texture_region == nullptr)
	{
		stream.pop(false);
		return false;
	}

	int cell_width = result.texture_region->get_width() / chomsky::config::FONT_NUM_COLUMNS;
	int cell_height = result.texture_region->get_height() / chomsky::config::FONT_NUM_ROWS;

	float s = result.element.get_vertex(GUIElement::TOP_LEFT).texture_coordinate.s;
	float t = result.element.get_vertex(GUIElement::TOP_LEFT).texture_coordinate.t;

	int x = s * result.texture.get_width() - result.texture_region->get_x();
	int y = t * result.texture.get_height() - result.texture_region->get_y();

	int row = x / cell_width;
	int column = y / cell_height;

	char c = column * chomsky::config::FONT_NUM_COLUMNS + row;
	auto& font_manager = parser.get_font_extent_manager();
	if (!font_manager.has_extents(result.element.get_key()) &&
		!font_manager.generate_extents(result.element.get_key(), *result.texture_region))
	{
		stream.pop(false);
		return false;
	}

	result.size = font_manager.get_size(result.element.get_key(), c);
	result.offset = font_manager.get_offset(result.element.get_key(), c);
	result.character = c;

	stream.pop(true);
	return true;
}

bool chomsky::parser::MatchText::is_space(const std::string& character)
{
	// XXX: Sometimes a NBSP (ISO/IEC 8859-13 I presume) is inserted
	// into the stream. Handle this appropriately.
	return character == " " || character == "\xa0";
}
