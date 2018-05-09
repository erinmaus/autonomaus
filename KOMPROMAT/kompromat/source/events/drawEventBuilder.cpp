// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <glm/gtc/type_ptr.hpp>
#include "kompromat/events/drawEventBuilder.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/events/drawEvent.hpp"

kompromat::DrawEventBuilder::DrawEventBuilder(kompvter::DrawEvent& event)
	: event(event)
{
	// Nil.
}

void kompromat::DrawEventBuilder::process(
	const paley::Event& e,
	kompvter::BufferManager& buffer_manager)
{
	process_draw_parameters(e);
	process_vertex_attributes(e, buffer_manager);
	process_vertex_index_mapper(e, buffer_manager);

	glm::mat4 projection_matrix;
	process_matrix(e, "projection_matrix", projection_matrix);
	this->event.set_projection_matrix(projection_matrix);

	glm::mat4 view_matrix;
	process_matrix(e, "view_matrix", view_matrix);
	this->event.set_view_matrix(view_matrix);

	glm::mat4 model_matrix;
	process_matrix(e, "model_matrix", model_matrix);
	this->event.set_model_matrix(model_matrix);

}

void kompromat::DrawEventBuilder::process_draw_parameters(const paley::Event& e)
{
	int offset_start, offset_end;
	{
		paley::EventField o;
		if (e.get_root().try_get_field("offset", o))
		{
			paley::EventField o_s;
			if (!o.try_get_field("start", o_s)
				|| !o_s.try_get_integer(offset_start))
			{
				assert(false);
				offset_start = 0;
			}

			paley::EventField o_e;
			if (!o.try_get_field("end", o_e)
				|| !o_e.try_get_integer(offset_end))
			{
				assert(false);
				offset_end = 0;
			}

			assert(offset_start >= 0);
			assert(offset_end >= 0);
			assert(offset_end >= offset_start);
		}
		else
		{
			assert(false);
			offset_start = 0;
			offset_end = 0;
		}
	}

	this->event.set_draw_parameters(kompvter::DrawParameters(
		kompvter::DrawParameters::primitive_triangles,
		offset_start, offset_end));
}

void kompromat::DrawEventBuilder::process_vertex_index_mapper(
	const paley::Event& e,
	kompvter::BufferManager& buffer_manager)
{
	int index_buffer;
	{
		paley::EventField i;
		if (!e.get_root().try_get_field("index_buffer", i) ||
			!i.try_get_integer(index_buffer))
		{
			assert(false);
			index_buffer = 0;
		}
	}

	int format;
	{
		paley::EventField f;
		std::string enumeration;
		if (!e.get_root().try_get_field("index_format", f) ||
			!f.try_get_string(enumeration))
		{
			assert(false);
			enumeration = "GL_NONE";
		}

		if (enumeration == "GL_NONE")
		{
			format = kompvter::VertexIndexMapper::format_unsigned_byte;
		}
		else if (enumeration == "GL_UNSIGNED_BYTE")
		{
			format = kompvter::VertexIndexMapper::format_unsigned_byte;
		}
		else if (enumeration == "GL_UNSIGNED_SHORT")
		{
			format = kompvter::VertexIndexMapper::format_unsigned_short;
		}
		else if (enumeration == "GL_UNSIGNED_INT")
		{
			format = kompvter::VertexIndexMapper::format_unsigned_integer;
		}
		else
		{
			assert(false);
			format = kompvter::VertexIndexMapper::format_unsigned_byte;
		}
	}

	auto buffer = buffer_manager.get_buffer(index_buffer);
	this->event.set_vertex_index_mapper(
		kompvter::VertexIndexMapper(buffer, format));
}

void kompromat::DrawEventBuilder::process_vertex_attributes(
	const paley::Event& e,
	kompvter::BufferManager& buffer_manager)
{
	paley::EventField a;
	if (!e.get_root().try_get_field("vertex_attributes", a))
	{
		assert(false);
		a.make_table();
	}

	kompvter::VertexAttributes attributes;
	auto current = a.get_first_field();
	paley::EventField i;
	while (a.try_get_field(current, i))
	{
		process_vertex_attribute(attributes, current, i, buffer_manager);
		current = a.get_next_field(current);
	}

	this->event.set_vertex_attributes(attributes);
}

void kompromat::DrawEventBuilder::process_vertex_attribute(
	kompvter::VertexAttributes& attributes,
	const std::string& name,
	const paley::EventField& attribute,
	kompvter::BufferManager& buffer_manager)
{
	int buffer_name;
	{
		paley::EventField b;
		if (!attribute.try_get_field("buffer", b) ||
			!b.try_get_integer(buffer_name))
		{
			assert(false);
			buffer_name = 0;
		}
	}

	int num_components;
	{
		paley::EventField n;
		if (!attribute.try_get_field("num_components", n) ||
			!n.try_get_integer(num_components))
		{
			assert(false);
			num_components = 0;
		}
	}

	int stride;
	{
		paley::EventField s;
		if (!attribute.try_get_field("stride", s) ||
			!s.try_get_integer(stride))
		{
			assert(false);
			stride = 0;
		}
	}

	int offset;
	{
		paley::EventField o;
		if (!attribute.try_get_field("offset", o) ||
			!o.try_get_integer(offset))
		{
			assert(false);
			offset = 0;
		}
	}

	bool normalized;
	{
		paley::EventField n;
		if (!attribute.try_get_field("normalized", n) ||
			!n.try_get_boolean(normalized))
		{
			assert(false);
			normalized = false;
		}
	}

	int format;
	{
		paley::EventField f;
		std::string enumeration;
		if (!attribute.try_get_field("format", f) ||
			!f.try_get_string(enumeration))
		{
			assert(false);
			enumeration = "GL_NONE";
		}

		if (enumeration == "GL_BYTE")
		{
			format = kompvter::VertexFetcher::format_byte;
		}
		else if (enumeration == "GL_UNSIGNED_BYTE")
		{
			format = kompvter::VertexFetcher::format_unsigned_byte;
		}
		else if (enumeration == "GL_SHORT")
		{
			format = kompvter::VertexFetcher::format_short;
		}
		else if (enumeration == "GL_UNSIGNED_SHORT")
		{
			format = kompvter::VertexFetcher::format_unsigned_short;
		}
		else if (enumeration == "GL_INT")
		{
			format = kompvter::VertexFetcher::format_integer;
		}
		else if (enumeration == "GL_UNSIGNED_INT")
		{
			format = kompvter::VertexFetcher::format_unsigned_integer;
		}
		else if (enumeration == "GL_FLOAT")
		{
			format = kompvter::VertexFetcher::format_float;
		}
		else if (enumeration == "GL_NONE")
		{
			format = kompvter::VertexFetcher::format_none;
		}
		else
		{
			assert(false);
			format = kompvter::VertexFetcher::format_none;
		}
	}

	auto buffer = buffer_manager.get_buffer(buffer_name);
	kompvter::VertexFetcher fetcher(
		buffer, num_components, format, normalized, stride, offset);
	attributes.add(name, fetcher);
}

void kompromat::DrawEventBuilder::process_matrix(
	const paley::Event& e,
	const std::string& field,
	glm::mat4& result)
{
	paley::EventField m;
	if (e.get_root().try_get_field(field, m))
	{
		const std::uint8_t* data;
		std::size_t data_size;

		if (m.try_get_data(data, data_size) && data_size == sizeof(glm::mat4))
		{
			auto d = glm::value_ptr(result);
			std::memcpy(d, data, data_size);
		}
	}
}
