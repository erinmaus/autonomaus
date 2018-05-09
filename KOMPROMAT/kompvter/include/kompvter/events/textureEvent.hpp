// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_EVENTS_TEXTURE_UPDATE_EVENT_HPP
#define KOMPVTER_EVENTS_TEXTURE_UPDATE_EVENT_HPP

#include <cstddef>
#include <string>
#include <glm/glm.hpp>

namespace kompvter
{
	class TextureEvent
	{
	public:
		TextureEvent() = default;
		virtual ~TextureEvent() = default;

		int get_name() const;
		void set_name(int value);

		int get_destination_name() const;
		void set_destination_name(int value);

		int get_source_name() const;
		void set_source_name(int value);

		enum
		{
			component_format_none = 0,
			component_format_r,
			component_format_rg,
			component_format_rgb,
			component_format_rgba
		};
		int get_component_format() const;
		void set_component_format(int value);

		enum
		{
			component_type_none = 0,
			component_type_unsigned_byte,
			component_type_unsigned_short,
			component_type_unsigned_integer,
			component_type_float
		};
		int get_component_type() const;
		void set_component_type(int value);

		enum
		{
			compression_none = 0,
			compression_rgb_dxt1,
			compression_rgba_dxt1,
			compression_rgba_dxt3,
			compression_rgba_dxt5,
			compression_srgb_dxt1,
			compression_srgba_dxt1,
			compression_srgba_dxt3,
			compression_srgba_dxt5,
		};
		int get_compression() const;
		void set_compression(int value);
		bool get_is_compressed() const;

		int get_width() const;
		void set_width(int value);

		int get_height() const;
		void set_height(int value);

		int get_x() const;
		void set_x(int value);

		int get_y() const;
		void set_y(int value);

		int get_source_x() const;
		void set_source_x(int value);

		int get_source_y() const;
		void set_source_y(int value);

		int get_destination_x() const;
		void set_destination_x(int value);

		int get_destination_y() const;
		void set_destination_y(int value);

		const std::uint8_t* get_pixels_data() const;
		void set_pixels_data(const std::uint8_t* value);

		std::size_t get_pixels_data_size() const;
		void set_pixels_data_size(std::size_t value);

	private:
		int destination = -1, source = -1;

		int component_format = component_format_none;
		int component_type = component_type_none;
		int compression = compression_none;

		int width = 0, height = 0;
		int source_x = 0, source_y = 0;
		int destination_x = 0, destination_y = 0;

		const std::uint8_t* pixels_data = nullptr;
		std::size_t pixels_data_size = 0;
	};
}

#endif
