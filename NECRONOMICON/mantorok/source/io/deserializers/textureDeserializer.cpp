// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/deserializers/textureDeserializer.hpp"
#include "mantorok/io/deserializers/textureRegionDeserializer.hpp"

void mantorok::DATDeserializer<kompvter::Texture>::deserialize(
	kompvter::Texture& texture, DATReader& reader)
{
	std::int32_t name;
	std::uint32_t width, height;
	std::int32_t is_compressed;
	std::int32_t pixel_format, pixel_type;

	if (!reader.match_section("kompvter.Texture::root"))
	{
		return;
	}

	reader.read(name);
	reader.read(width);
	reader.read(height);
	reader.read(is_compressed);
	reader.read(pixel_format);
	reader.read(pixel_type);

	texture = kompvter::Texture(
		name,
		width, height,
		pixel_format, pixel_type, is_compressed);
	if (reader.match_section("kompvter.Texture::regions"))
	{
		DATReader::Section current_region;
		while (reader.try_peek_section(current_region))
		{
			if (current_region.name != "kompvter.Texture::region")
			{
				break;
			}
			reader.enter_section();

			std::int32_t x, y;
			reader.read(x);
			reader.read(y);

			kompvter::TextureRegion region;
			DATDeserializer<kompvter::TextureRegion>::deserialize(region, reader);
			texture.add_region(
				x, y,
				region.get_width(), region.get_height(),
				region.get_pixel_format(), region.get_pixel_type(),
				region.get_is_compressed(),
				region.get_pixels_data(), region.get_pixels_size());

			reader.leave_section();
		}

		reader.leave_section();
	}
}
