// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include "chomsky/gui/guiElement.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiStateBuilder.hpp"
#include "chomsky/gui/parser/print.hpp"

bool chomsky::parser::Print::process(
	GUIParser& parser,
	GUIElementStream& stream,
	GUIStateBuilder& state_builder) const
{
	stream.push();

	GUIElement element;
	if (!stream.next(element))
	{
		stream.pop(false);
		return false;
	}

	if (element.get_is_textured())
	{
		std::printf(
			"element %lu (key: %s)\n",
			element.get_offset(),
			element.get_key().c_str());
	}
	else
	{
		std::printf("element %lu (untextured)\n", element.get_offset());
	}

	auto& dictionary = parser.get_texture_dictionary();
	if (dictionary.has(element.get_key()))
	{
		auto& entry = dictionary.get(element.get_key());
		std::printf("\ttexture: %s\n", entry.get_value().c_str());
	}

	std::printf("\tposition: (%d, %d)\n", element.get_x(), element.get_y());
	std::printf("\tsize: (%d, %d)\n", element.get_width(), element.get_height());
	std::printf("\trotation: %.0f\n", element.get_rotation() * (180 / M_PI));

	for (std::size_t i = 0; i < element.get_num_vertices(); ++i)
	{
		auto vertex = element.get_vertex(i);
		std::printf(
			"\tvertex[%lu]: { x: %d, y: %d, s: %f, t: %f, color: %02x%02x%02x%02x }\n",
			i,
			vertex.position.x, vertex.position.y,
			vertex.texture_coordinate.s, vertex.texture_coordinate.t,
			(int)(vertex.color.r * 255), (int)(vertex.color.g * 255), (int)(vertex.color.b * 255), (int)(vertex.color.a * 255));
	}

	stream.pop(false);
	return true;
}
