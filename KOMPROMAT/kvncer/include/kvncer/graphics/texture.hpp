// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVNCER_GRAPHICS_TEXTURE_HPP
#define KVNCER_GRAPHICS_TEXTURE_HPP

#include <cstddef>
#include <cstdint>
#include "kompvter/events/textureEvent.hpp"
#include "kvncer/render/gl.hpp"

namespace kvncer
{
	class Texture
	{
	public:
		enum
		{
			component_format_r = kompvter::TextureEvent::component_format_r,
			component_format_rg = kompvter::TextureEvent::component_format_rg,
			component_format_rgb = kompvter::TextureEvent::component_format_rgb,
			component_format_rgba = kompvter::TextureEvent::component_format_rgba
		};

		enum
		{
			component_type_unsigned_byte = kompvter::TextureEvent::component_type_unsigned_byte,
			component_type_unsigned_short = kompvter::TextureEvent::component_type_unsigned_short,
			component_type_unsigned_integer = kompvter::TextureEvent::component_type_unsigned_integer,
			component_type_float = kompvter::TextureEvent::component_type_float
		};

		enum
		{
			compression_rgb_dxt1 = kompvter::TextureEvent::compression_rgb_dxt1,
			compression_rgba_dxt1 = kompvter::TextureEvent::compression_rgba_dxt1,
			compression_rgba_dxt3 = kompvter::TextureEvent::compression_rgba_dxt3,
			compression_rgba_dxt5 = kompvter::TextureEvent::compression_rgba_dxt5,
			compression_srgb_dxt1 = kompvter::TextureEvent::compression_srgb_dxt1,
			compression_srgba_dxt1 = kompvter::TextureEvent::compression_srgba_dxt1,
			compression_srgba_dxt3 = kompvter::TextureEvent::compression_srgba_dxt3,
			compression_srgba_dxt5 = kompvter::TextureEvent::compression_srgba_dxt5
		};

		Texture(int format, bool is_compressed, int width, int height);
		~Texture();

		void update(
			int x, int y, int width, int height,
			int component_format, int component_type,
			const std::uint8_t* pixel_data);
		void compressed_update(
			int x, int y, int width, int height,
			int compression_format,
			const std::uint8_t* pixel_data, std::size_t pixel_data_size);

		void bind(int unit);

		GLuint get_texture() const;
		int get_width() const;
		int get_height() const;

	private:
		int width, height;

		GLuint texture;
	};
}

#endif
