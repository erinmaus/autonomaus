// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompromat/events/textureEventBuilder.hpp"

const kompvter::TextureEvent& kompromat::TextureEventBuilder::get_event() const
{
	return this->event;
}

void kompromat::TextureEventBuilder::from_texture_create(const paley::Event& e)
{
	int name;
	{
		paley::EventField n;
		if (!e.get_root().try_get_field("name", n) ||
			!n.try_get_integer(name))
		{
			assert(false);
			name = 0;
		}
	}
	this->event.set_name(name);
}

void kompromat::TextureEventBuilder::from_texture_delete(const paley::Event& e)
{
	int name;
	{
		paley::EventField n;
		if (!e.get_root().try_get_field("name", n) ||
			!n.try_get_integer(name))
		{
			assert(false);
			name = 0;
		}
	}
	this->event.set_name(name);
}

void kompromat::TextureEventBuilder::from_texture_allocate(const paley::Event& e)
{
	int name;
	{
		paley::EventField n;
		if (!e.get_root().try_get_field("name", n) ||
			!n.try_get_integer(name))
		{
			assert(false);
			name = 0;
		}
	}
	this->event.set_name(name);

	process_dimensions(e.get_root(), "width", "height");

	bool is_compressed;
	{
		paley::EventField b;
		if (!e.get_root().try_get_field("is_compressed", b) ||
			!b.try_get_boolean(is_compressed))
		{
			assert(false);
			is_compressed = false;
		}
	}

	if (is_compressed)
	{
		process_compression_format(e.get_root(), "compression_format");
	}
	
	process_component_format(e.get_root(), "format");
}

void kompromat::TextureEventBuilder::from_texture_update(const paley::Event& e)
{
	int name;
	{
		paley::EventField n;
		if (!e.get_root().try_get_field("name", n) ||
			!n.try_get_integer(name))
		{
			assert(false);
			name = 0;
		}
	}
	this->event.set_name(name);

	const std::uint8_t* pixels_data;
	std::size_t pixels_data_size;
	if (!e.get_root().try_get_field("data", this->event_data) ||
		!this->event_data.try_get_data(pixels_data, pixels_data_size))
	{
		assert(false);
		pixels_data = nullptr;
		pixels_data_size = 0;
	}
	this->event.set_pixels_data(pixels_data);
	this->event.set_pixels_data_size(pixels_data_size);

	{
		paley::EventField s;
		if (!e.get_root().try_get_field("size", s))
		{
			assert(false);
			s.make_table();
		}

		process_dimensions(s, "x", "y");
	}

	bool is_compressed;
	{
		paley::EventField b;
		if (!e.get_root().try_get_field("is_compressed", b) ||
			!b.try_get_boolean(is_compressed))
		{
			assert(false);
			is_compressed = false;
		}
	}

	if (is_compressed)
	{
		process_compression_format(e.get_root(), "format");
	}
	else
	{
		process_component_format(e.get_root(), "component_format");
		process_component_type(e.get_root(), "component_type");
	}
}

void kompromat::TextureEventBuilder::from_texture_copy(const paley::Event& e)
{
	process_dimensions(e.get_root(), "width", "height");

	int destination;
	int destination_x, destination_y;
	{
		paley::EventField d;
		if (!e.get_root().try_get_field("destination", d))
		{
			assert(false);
			d.make_table();
		}

		paley::EventField n;
		if (!d.try_get_field("name", n) ||
			!n.try_get_integer(destination))
		{
			assert(false);
			destination = 0;
		}

		process_offset(d, destination_x, destination_y);
	}
	this->event.set_destination_name(destination);
	this->event.set_destination_x(destination_x);
	this->event.set_destination_y(destination_y);

	int source;
	int source_x, source_y;
	{
		paley::EventField s;
		if (!e.get_root().try_get_field("source", s))
		{
			assert(false);
			s.make_table();
		}

		paley::EventField n;
		if (!s.try_get_field("name", n) ||
			!n.try_get_integer(source))
		{
			assert(false);
			source = 0;
		}

		process_offset(s, source_x, source_y);
	}
	this->event.set_source_name(source);
	this->event.set_source_x(source_x);
	this->event.set_source_y(source_y);
}

void kompromat::TextureEventBuilder::process_component_format(
	const paley::EventField& root,
	const std::string& field)
{
	std::string value;
	{
		paley::EventField v;
		if (!root.try_get_field(field, v) ||
			!v.try_get_string(value))
		{
			assert(false);
			value = "GL_NONE";
		}
	}

	if (value == "GL_NONE")
	{
		this->event.set_component_format(
			kompvter::TextureEvent::component_format_none);
	}
	else if (value == "GL_RED")
	{
		this->event.set_component_format(
			kompvter::TextureEvent::component_format_r);
	}
	else if (value == "GL_RG")
	{
		this->event.set_component_format(
			kompvter::TextureEvent::component_format_rg);
	}
	else if (value == "GL_RGB")
	{
		this->event.set_component_format(
			kompvter::TextureEvent::component_format_rgb);
	}
	else if (value == "GL_RGBA")
	{
		this->event.set_component_format(
			kompvter::TextureEvent::component_format_rgba);
	}
	else
	{
		assert(false);
		this->event.set_component_format(
			kompvter::TextureEvent::component_format_none);
	}
}

void kompromat::TextureEventBuilder::process_compression_format(
	const paley::EventField& root,
	const std::string& field)
{
	std::string format;
	{
		paley::EventField f;
		if (!root.try_get_field(field, f) || !f.try_get_string(format))
		{
			assert(false);
			format = "GL_NONE";
		}
	}

	if (format == "GL_NONE")
	{
		this->event.set_compression(
			kompvter::TextureEvent::compression_none);
	}
	else if (format == "GL_COMPRESSED_RGB_S3TC_DXT1_EXT")
	{
		this->event.set_compression(
			kompvter::TextureEvent::compression_rgb_dxt1);
	}
	else if (format == "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT")
	{
		this->event.set_compression(
			kompvter::TextureEvent::compression_rgba_dxt1);
	}
	else if (format == "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT")
	{
		this->event.set_compression(
			kompvter::TextureEvent::compression_rgba_dxt3);
	}
	else if (format == "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT")
	{
		this->event.set_compression(
			kompvter::TextureEvent::compression_rgba_dxt5);
	}
	else if (format == "GL_COMPRESSED_SRGB_S3TC_DXT1_EXT")
	{
		this->event.set_compression(
			kompvter::TextureEvent::compression_srgb_dxt1);
	}
	else if (format == "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT")
	{
		this->event.set_compression(
			kompvter::TextureEvent::compression_srgba_dxt1);
	}
	else if (format == "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT")
	{
		this->event.set_compression(
			kompvter::TextureEvent::compression_srgba_dxt3);
	}
	else if (format == "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT")
	{
		this->event.set_compression(
			kompvter::TextureEvent::compression_srgba_dxt5);
	}
	else
	{
		assert(false);
		this->event.set_compression(
			kompvter::TextureEvent::compression_none);
	}
}

void kompromat::TextureEventBuilder::process_component_type(
	const paley::EventField& root,
	const std::string& field)
{
	std::string value;
	{
		paley::EventField v;
		if (!root.try_get_field(field, v) ||
			!v.try_get_string(value))
		{
			assert(false);
			value = "GL_NONE";
		}
	}

	if (value == "GL_NONE")
	{
		this->event.set_component_type(
			kompvter::TextureEvent::component_type_none);
	}
	else if (value == "GL_UNSIGNED_BYTE")
	{
		this->event.set_component_type(
			kompvter::TextureEvent::component_type_unsigned_byte);
	}
	else if (value == "GL_UNSIGNED_SHORT")
	{
		this->event.set_component_type(
			kompvter::TextureEvent::component_type_unsigned_short);
	}
	else if (value == "GL_UNSIGNED_INT")
	{
		this->event.set_component_type(
			kompvter::TextureEvent::component_type_unsigned_integer);
	}
	else if (value == "GL_FLOAT")
	{
		this->event.set_component_type(
			kompvter::TextureEvent::component_type_float);
	}
	else
	{
		assert(false);
		this->event.set_component_type(
			kompvter::TextureEvent::component_type_none);
	}
}

void kompromat::TextureEventBuilder::process_dimensions(
	const paley::EventField& root,
	const std::string& width_field,
	const std::string& height_field)
{
	int width;
	{
		paley::EventField w;
		if (!root.try_get_field(width_field, w) ||
			!w.try_get_integer(width))
		{
			assert(false);
			width = 0;
		}
	}
	this->event.set_width(width);

	int height;
	{
		paley::EventField w;
		if (!root.try_get_field(height_field, w) ||
			!w.try_get_integer(height))
		{
			assert(false);
			height = 0;
		}
	}
	this->event.set_height(height);
}


void kompromat::TextureEventBuilder::process_offset(
	const paley::EventField& root,
	int& result_x, int& result_y)
{
	paley::EventField x;
	if (!root.try_get_field("x", x) || !x.try_get_integer(result_x))
	{
		assert(false);
		result_x = 0;
	}

	paley::EventField y;
	if (!root.try_get_field("y", y) || !y.try_get_integer(result_y))
	{
		assert(false);
		result_y = 0;
	}
}
